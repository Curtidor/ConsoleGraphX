#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <memory>
#include "Engine\Graphics\ScreenGraphics\screen.h"
#include "Engine\Core\Window\window.h"
#include "Engine\Core\Concurrency\thread_manager.h"

namespace ConsoleGraphX_Internal
{
    class LoggerManager
    {
    public:
        enum class LogLevel
        {
            CGX_INFO = 1,
            CGX_WARNING = 2,
            CGX_ERROR = 3
        };

    public:
        LoggerManager();
        ~LoggerManager();

        static void Initialize();
        static LoggerManager& Instance();
        static void ShutDown();

        void StartLoggerThread(ConsoleGraphX_Internal::ThreadManager& threadManager); 

        void LogMessage(const std::string& loggerName, const std::string& message, LogLevel level = LogLevel::CGX_INFO);
        void AttachWindow(ConsoleGraphX::CrossProcessWindow* window);
        void DetachWindow(ConsoleGraphX::AbstractWindow* window);

    private:
        void _ProcessQueue(std::atomic<bool>& shouldQuit);
        void _FormatLogMessage(std::string& message, LogLevel level);

    private:
        static LoggerManager* _s_instance;
       
        bool _m_loggingThreadStarted;

        std::mutex _m_mutex;
        std::condition_variable _m_cv;

        std::queue<std::string> _m_messageQueue;
        const size_t _m_maxMessages = 250;

        ConsoleGraphX::CrossProcessWindow* _m_loggerWindow;

        std::atomic<bool> _m_terminate;
        std::thread::id _m_threadID; // no longer owns the thread
    };

};
