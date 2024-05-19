#pragma once
#include "debugger.h"
#include "system_manager.h"

namespace ConsoleGraphX
{
	class Engine
	{
	private:
		static ConsoleGraphX_Interal::SystemManager* _s_systemManager;
		static ConsoleGraphX_Interal::Debugger* _s_debugger;

		static bool _s_isRunning;

		static void _Update(float deltaTime);


	public:
		static void InitializeEngine(short screen_width, short screen_height, short pixel_width, short pixel_height, ConsoleGraphX_Interal::Debugger& debugger);
		static void Run();
	};
};
