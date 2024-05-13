#pragma once
#include <deque>
#include <string>

namespace ConsoleGraphX_Interal
{
    class Debugger
    {
    public:
        enum class LogLevel
        {
            INFO = 1,
            WARNING = 2,
            ERRORd = 3
        };

        Debugger(int max_messages);

        void LogMessage(const std::string& message, LogLevel level = LogLevel::INFO);
        void DisplayMessages();
        static void S_LogMessage(const std::string& message, LogLevel level = LogLevel::INFO);

        int GetMaxMessages();

    private:
        static Debugger* _s_active_debugger;

        int m_max_messages;

        std::deque<std::string> m_message_queue;
        std::string GetFormattedLogMessage(const std::string& message, LogLevel level);
    };

};
