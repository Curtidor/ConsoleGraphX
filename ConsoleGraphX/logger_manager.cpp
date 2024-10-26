#include "PCH_CGX.h"
#include <handleapi.h>
#include <errhandlingapi.h>
#include <processthreadsapi.h>
#include <thread>
#include <mutex>
#include <cassert>
#include "logger_manager.h"
#include "screen.h"
#include "window.h"


namespace ConsoleGraphX_Internal
{
     LoggerManager::LoggerManager(const std::string& debuggerName) :
         _m_terminate(false), _m_engineWindow(ConsoleGraphX::EngineWindow(80, 40, debuggerName, 20, 12))
     {
        _m_thread = std::thread(&LoggerManager::_ProcessQueue, this);
     }

    LoggerManager::~LoggerManager()
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

    void LoggerManager::Initialize()
    {
        assert(!_s_instance);

        _s_instance = new LoggerManager("Logger");
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

    void LoggerManager::LogMessage(const std::string& loggerName, const std::string& message, LogLevel level)
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

    void LoggerManager::_ProcessQueue()
    {
        static int y = 0;
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
            
            y = _m_engineWindow.WriteText(message, 2, y++) ? 0 : y++;
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
            message.insert(0, "[WARNING]" );
            break;
        case LogLevel::CGX_ERROR:
            message.insert(0, "[ERROR]");
            break;
        }
    }
}
