
#include "debugger.h"

Debugger* Debugger::_s_active_debugger;

Debugger::Debugger(int max_messages)
    : m_max_messages(max_messages)
{
    this->_s_active_debugger = this;
}

void Debugger::LogMessage(const std::string& message, LogLevel level)
{
    if (this->m_message_queue.size() >= this->m_max_messages)
    {
        this->m_message_queue.pop_back();
    }

    std::string formattedMessage = GetFormattedLogMessage(message, level);

    this->m_message_queue.push_front(formattedMessage);
}

void Debugger::S_LogMessage(const std::string& message, LogLevel level)
{
    Debugger::_s_active_debugger->LogMessage(message, level);
}


void Debugger::DisplayMessages()
{
    int y = 1;
    for (const std::string& message : this->m_message_queue)
    {
        Screen::SetText_A(0, Screen::GetHeight_A() + y++, message);
    }
}

std::string Debugger::GetFormattedLogMessage(const std::string& message, LogLevel level)
{
    std::string formattedMessage;

    // Get the current time and format it
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &time_t);
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &tm);

    formattedMessage += "[" + std::string(timeStr) + "] ";

    // Add log level
    switch (level)
    {
    case LogLevel::INFO:
        formattedMessage += "[INFO] ";
        break;
    case LogLevel::WARNING:
        formattedMessage += "[WARNING] ";
        break;
    case LogLevel::ERRORd:
        formattedMessage += "[ERROR] ";
        break;
    default:
        break;
    }

    formattedMessage += message;

    return formattedMessage;
}

int Debugger::GetMaxMessages()
{
    return this->m_max_messages;;
}
