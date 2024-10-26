#pragma once
#include <string>
#include "logger_manager.h"



namespace ConsoleGraphX_Internal
{
	class Logger
	{
	private:
		const std::string _m_name;

	public:
		Logger(const std::string& name);
		void LogMessage(const std::string& message, LoggerManager::LogLevel level = LoggerManager::LogLevel::CGX_INFO) const;

	};
}
