#pragma once
#include <string>
#include "Engine\Core\Logger\logger_manager.h"



namespace ConsoleGraphX_Internal
{
	class Logger
	{
	private:
		std::string _m_name;

	public:
		Logger(const std::string& name);
		void LogMessage(const std::string& message, LoggerManager::LogLevel level = LoggerManager::LogLevel::CGX_INFO) const;

	};
}
