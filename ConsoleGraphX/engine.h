#pragma once
#include <chrono>
// utilities
#include "debugger.h"
// systems
#include "system.h"

namespace ConsoleGraphX
{
	class Engine
	{
	private:
		static System* _m_system;
		static ConsoleGraphX_Interal::Debugger* _m_debugger;

		static float _m_delta_time;
		static bool _m_is_running;

		static std::chrono::high_resolution_clock::time_point _m_prev_frame_time;
		static std::chrono::high_resolution_clock::time_point _m_current_frame_time;

	public:
		static void InitializeEngine(short screen_width, short screen_height, short pixel_width, short pixel_height, ConsoleGraphX_Interal::Debugger* debugger);
		static void Run();

		const static float DeltaTime();
		static void UpdateFPS();
	};
};
