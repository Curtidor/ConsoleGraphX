#pragma once
#include <deque>
#include <string>

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
    static void S_LogMessage(const std::string& message, LogLevel level = LogLevel::INFO);
    void DisplayMessages();

    int GetMaxMessages();

private:
    static Debugger* _s_active_debugger;

    int m_max_messages;
    std::deque<std::string> m_message_queue;

    std::string GetFormattedLogMessage(const std::string& message, LogLevel level);
};
