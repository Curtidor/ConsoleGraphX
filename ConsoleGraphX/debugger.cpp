#include <string>
#include <windows.h>
#include <memory>
#include <thread>
#include <handleapi.h>
#include <errhandlingapi.h>
#include <processthreadsapi.h>
#include <mutex>
#include <condition_variable>
#include "debugger.h"
#include "../IPC/sender.h"

namespace ConsoleGraphX_Internal
{
    Debugger* Debugger::_s_active_debugger = nullptr;

    Debugger::Debugger(const std::wstring& debuggerName) : _m_sender(std::make_unique<Sender<std::string>>(debuggerName)), _m_terminate(false)
    {
        _s_active_debugger = this;
        _m_thread = std::thread(&Debugger::_ProcessQueue, this);

        _StartDebuggerReceiver();
    }

    Debugger::~Debugger()
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

    void Debugger::LogMessage(const std::string& message, LogLevel level)
    {
        std::unique_lock<std::mutex> lock(_m_mutex);
       
        std::string formattedMessage = _GetFormattedLogMessage(message, level);
        _m_messageQueue.push(formattedMessage);
        
        _m_cv.notify_one();
    }

    void Debugger::_ProcessQueue()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(_m_mutex);
            _m_cv.wait(lock, [this]() { return !_m_messageQueue.empty() || _m_terminate; });

            if (_m_terminate && _m_messageQueue.empty())
                break;

            while (!_m_messageQueue.empty())
            {
                _m_sender->SendMessageIPC(_m_messageQueue.front());
                _m_messageQueue.pop();
            }
        }
    }

    void Debugger::S_LogMessage(const std::string& message, LogLevel level)
    {
        if (_s_active_debugger != nullptr)
            _s_active_debugger->LogMessage(message, level);
    }

    std::string Debugger::_GetFormattedLogMessage(const std::string& message, LogLevel level)
    {
        std::string levelStr;
        switch (level)
        {
        case LogLevel::CGX_INFO:
            levelStr = "INFO";
            break;
        case LogLevel::CGX_WARNING:
            levelStr = "WARNING";
            break;
        case LogLevel::CGX_ERROR:
            levelStr = "ERROR";
            break;
        }
        return "[" + levelStr + "] " + message;
    }

    void Debugger::_StartDebuggerReceiver()
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
            LPVOID lpMsgBuf;
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
}
