#include "PCH_CGX.h"
#include "Engine\Core\Logger\logger.h"
#include "Engine\Core\Logger\logger_manager.h"

namespace ConsoleGraphX_Internal
{

	Logger::Logger(const std::string& name): _m_name(name)
	{}

	void Logger::LogMessage(const std::string& message, LoggerManager::LogLevel level) const
	{
		LoggerManager::Instance().LogMessage(_m_name, message, level);
	}
};

