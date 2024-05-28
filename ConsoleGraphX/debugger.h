#pragma once
#include <windows.h>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <memory>
#include <condition_variable>
#include "../IPC/sender.h"

namespace ConsoleGraphX_Internal
{
    class Debugger
    {
    private:
        static Debugger* _s_active_debugger;  // Singleton instance

        bool _m_terminate;  // Flag to terminate the queue processing
        int _m_maxMessages = 1000;
        std::mutex _m_mutex;  // Mutex for synchronizing access to the message queue
        std::thread _m_thread;  // Thread for processing the message queue
        std::condition_variable _m_cv;  // Condition variable for queue processing
        std::unique_ptr<Sender<std::string>> _m_sender;  // IPC sender
        std::queue<std::string> _m_messageQueue;  // Queue for log messages

        HANDLE _m_receiverProcessHandle;


    public:
        enum class LogLevel
        {
            CGX_INFO = 1,
            CGX_WARNING = 2,
            CGX_ERROR = 3  
        };

        // Constructor: Initializes the debugger with a name and starts the receiver
        Debugger(const std::wstring& debuggerName);

        // Destructor: Cleans up resources and terminates the queue processing thread
        ~Debugger();

        // Log a message with the specified log level (default: INFO)
        void LogMessage(const std::string& message, LogLevel level = LogLevel::CGX_INFO);

        // Static method to log a message using the active debugger instance
        static void S_LogMessage(const std::string& message, LogLevel level = LogLevel::CGX_INFO);

    private:
        // Process the message queue in a separate thread
        void _ProcessQueue();

        // Starts the debugger receiver process
        void _StartDebuggerReceiver();

        // Formats the log message with the specified log level
        void _FormatLogMessage(std::string& message, LogLevel level);
    };
}
