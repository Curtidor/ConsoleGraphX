#include "PCH_CGX.h"
#include <handleapi.h>
#include <errhandlingapi.h>
#include <processthreadsapi.h>
#include <thread>
#include <mutex>
#include <cassert>
#include "Engine\Core\Logger\logger_manager.h"
#include "Engine\Graphics\ScreenGraphics\screen.h"
#include "Engine\Core\Window\window.h"
#include "Engine\Core\Window\window_manager.h"
#include "Engine\Core\Concurrency\thread_manager.h"

namespace ConsoleGraphX_Internal
{
    LoggerManager* LoggerManager::_s_instance = nullptr;

    LoggerManager::LoggerManager()
        : _m_terminate(false), _m_loggerWindow(nullptr)
    {
    }

    LoggerManager::~LoggerManager()
    {
        {
            std::lock_guard<std::mutex> lock(_m_mutex);
            _m_terminate = true;
        }

        _m_cv.notify_all(); // wake logger thread if sleeping
    }


    void LoggerManager::Initialize(ThreadManager& threadManager)
    {
        assert(!_s_instance);
        _s_instance = new LoggerManager();
        _s_instance->StartLoggerThread(threadManager);
    }

    LoggerManager& LoggerManager::Instance()
    {
        assert(_s_instance);
        return *_s_instance;
    }

    void LoggerManager::ShutDown()
    {
        delete _s_instance;
        _s_instance = nullptr;
    }

    void LoggerManager::StartLoggerThread(ThreadManager& threadManager)
    {
       _m_threadID = threadManager.StartThread("LoggerThread", [this](std::atomic<bool>& shouldQuit) {
            _ProcessQueue(shouldQuit);
            });
    }

    void LoggerManager::LogMessage(const std::string& loggerName, const std::string& message, LogLevel level)
    {
    
        #if defined(DEBUG) && (MIN_BUILD == 1)
            return;
        #endif

        std::string formattedMessage;
        formattedMessage.reserve(loggerName.size() + message.size() + 14);

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

    void LoggerManager::AttachWindow(ConsoleGraphX::CrossProcessWindow* window)
    {
        CGX_VERIFY(window, "null Window!");
        _m_loggerWindow = window;

        [[maybe_unused]]
        ConsoleGraphX::EventCallBackHandle handle = window->OnWindowDestroyed.AddListener(this, &LoggerManager::DetachWindow);
    }

    void LoggerManager::DetachWindow(ConsoleGraphX::AbstractWindow* window)
    {
        _m_loggerWindow = nullptr;
    }

    void LoggerManager::_ProcessQueue(std::atomic<bool>& shouldQuit)
    {
        static uint16_t y = 0;
        while (!shouldQuit.load(std::memory_order_acquire))
        {
            std::string message;
            {
                std::unique_lock<std::mutex> lock(_m_mutex);
                _m_cv.wait(lock, [this, &shouldQuit]() {
                    return !_m_messageQueue.empty() || _m_terminate || shouldQuit.load();
                    });

                if ((_m_terminate || shouldQuit.load()) && _m_messageQueue.empty())
                    break;

                message = std::move(_m_messageQueue.front());
                _m_messageQueue.pop();
            }

            if (_m_loggerWindow != nullptr)
                y = _m_loggerWindow->WriteText(message, 2, y++) ? 0 : y++;
        }
    }

    void LoggerManager::_FormatLogMessage(std::string& message, LogLevel level)
    {
        switch (level)
        {
        case LogLevel::CGX_INFO:
            message.insert(0, "[INFO]");
            break;
        case LogLevel::CGX_WARNING:
            message.insert(0, "[WARNING]");
            break;
        case LogLevel::CGX_ERROR:
            message.insert(0, "[ERROR]");
            break;
        }
    }
}
