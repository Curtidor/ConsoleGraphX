#include <string>
#include "logger.h"
#include "debugger_manager.h"

namespace ConsoleGraphX_Internal
{

	Logger::Logger(const std::string& name): _m_name(name)
	{}

	void Logger::LogMessage(const std::string& message, DebuggerManager::LogLevel level) const
	{
		DebuggerManager::Instance().LogMessage(_m_name, message, level);
	}
};

