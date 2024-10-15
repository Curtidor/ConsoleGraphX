#include "CGXPCH.h"
#include <handleapi.h>
#include <errhandlingapi.h>
#include <processthreadsapi.h>
#include <thread>
#include <mutex>
#include <cassert>
#include "debugger_manager.h"
#include "../IPC/sender.h"

namespace ConsoleGraphX_Internal
{
     DebuggerManager::DebuggerManager(const std::wstring& debuggerName) : _m_sender(std::make_unique<Sender<std::string>>(debuggerName)), _m_terminate(false)
    {
        _m_thread = std::thread(&DebuggerManager::_ProcessQueue, this);

        _StartDebuggerReceiver();
    }

    DebuggerManager::~DebuggerManager()
    {
        {
            std::lock_guard<std::mutex> lock(_m_mutex);
            _m_terminate = true;
        }
        _m_cv.notify_one();
        if (_m_thread.joinable())
        {
            _m_thread.join();
        }
    }

    void DebuggerManager::Initialize()
    {
        assert(!_s_instance);

        _s_instance = new DebuggerManager(L"Debugger");
    }

    DebuggerManager& DebuggerManager::Instance()
    {
        assert(_s_instance);

        return *_s_instance;
    }

    void DebuggerManager::ShutDown()
    {
        _s_instance->_StopDebuggerReceiver();
        delete _s_instance;
        _s_instance = nullptr;
    }

    void DebuggerManager::LogMessage(const std::string& loggerName, const std::string& message, LogLevel level)
    {
        std::string formattedMessage;
        formattedMessage.reserve(loggerName.size() + message.size() + 14); // reserve space to avoid reallocations 14 is used because: 9 for max log level size, 5 for the added brackets and spaces ("[", "]", and the surrounding spaces).

        _FormatLogMessage(formattedMessage, level);

        formattedMessage += "[" + loggerName + "] " + message;
        {
            std::lock_guard<std::mutex> lock(_m_mutex);

            _m_messageQueue.push(std::move(formattedMessage));

            if (_m_messageQueue.size() > _m_maxMessages)
                _m_messageQueue.pop();
        }

        _m_cv.notify_one();
    }

    void DebuggerManager::_ProcessQueue()
    {
        while (true)
        {
            std::string message;
            {
                std::unique_lock<std::mutex> lock(_m_mutex);
                _m_cv.wait(lock, [this]() { return !_m_messageQueue.empty() || _m_terminate; });

                if (_m_terminate && _m_messageQueue.empty())
                    break;

                message = std::move(_m_messageQueue.front());
                _m_messageQueue.pop();
            }
            _m_sender->SendMessageIPC(message);
        }
    }

    void DebuggerManager::_FormatLogMessage(std::string& message, LogLevel level)
    {
        switch (level)
        {
        case LogLevel::CGX_INFO:
            message.insert(0, "[INFO]");
            break;
        case LogLevel::CGX_WARNING:
            message.insert(0, "[WARNING]" );
            break;
        case LogLevel::CGX_ERROR:
            message.insert(0, "[ERROR]");
            break;
        }
    }

    void DebuggerManager::_StartDebuggerReceiver()
    {
        std::wstring exePath;
        #ifdef _DEBUG
            exePath = L"../x64/Debug/DebuggerReceiver.exe";
        #else
            exePath = L"../x64/Release/DebuggerReceiver.exe";
        #endif

        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi = { 0 };

        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOWNORMAL;

        if (!CreateProcessW(exePath.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
        {
            DWORD error = GetLastError();
            LPVOID lpMsgBuf{};

            FormatMessageW(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                error,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPWSTR)&lpMsgBuf,
                0, NULL);

            LocalFree(lpMsgBuf);
            return;
        }

        _m_receiverProcessHandle = pi.hProcess;
        CloseHandle(pi.hThread);
    }

    void DebuggerManager::_StopDebuggerReceiver()
    {
        if (_m_receiverProcessHandle != nullptr)
        {
            TerminateProcess(_m_receiverProcessHandle, 0);
            CloseHandle(_m_receiverProcessHandle);
            _m_receiverProcessHandle = nullptr;
        }
    }
}
