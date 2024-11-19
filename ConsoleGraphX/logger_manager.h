#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <memory>
#include "screen.h"
#include "window.h"

namespace ConsoleGraphX_Internal
{
    class LoggerManager
    {
    private:
        static inline LoggerManager* _s_instance = nullptr;

        bool _m_terminate;  // flag to terminate the queue processing
        int _m_maxMessages = 1000;
        std::mutex _m_mutex;  // mutex for synchronizing access to the message queue
        std::thread _m_thread;  // thread for processing the message queue
        std::condition_variable _m_cv;  // condition variable for queue processing
        std::queue<std::string> _m_messageQueue; 
        HANDLE _m_receiverProcessHandle = nullptr;

        ConsoleGraphX::CrossProcessWindow* _m_loggerWindow;



    public:
        enum class LogLevel
        {
            CGX_INFO = 1,
            CGX_WARNING = 2,
            CGX_ERROR = 3
        };

        static void Initialize();
        static void ShutDown();
        static LoggerManager& Instance();

        LoggerManager(const char* loggerName);

        ~LoggerManager();

        // Log a message with the specified log level (default: INFO)
        void LogMessage(const std::string& loggerName, const std::string& message, LogLevel level = LogLevel::CGX_INFO);
        void AttachWindow(ConsoleGraphX::CrossProcessWindow* window);
        void DetachWindow(ConsoleGraphX::AbstractWindow* window);

    private:
        // Process the message queue in a separate thread
        void _ProcessQueue();
        // Formats the log message with the specified log level
        void _FormatLogMessage(std::string& message, LogLevel level);
    };
};
