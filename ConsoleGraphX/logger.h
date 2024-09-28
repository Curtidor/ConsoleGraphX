#pragma once
#include <string>
#include "debugger_manager.h"



namespace ConsoleGraphX_Internal
{
	class Logger
	{
	private:
		const std::string _m_name;

	public:
		Logger(const std::string& name);
		void LogMessage(const std::string& message, DebuggerManager::LogLevel level = DebuggerManager::LogLevel::CGX_INFO) const;

	};
}
