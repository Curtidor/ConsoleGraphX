#pragma once
#include <chrono>
#include <thread>
// utilities
#include "debugger.h"
// systems
#include "render_system.h"
#include "sprtite_system.h"
#include "input_system.h"
#include "player_controller_system.h"
#include "physics_system.h"
#include "screen.h"
#include "scene_system.h"
#include "script_system.h"


class Engine
{
private:
	static System* _m_system;
	static Debugger* _m_debugger;

	static bool _m_is_running;

public:
	static void InitializeEngine(short screen_width, short screen_height, short pixel_width, short pixel_height, Debugger* debugger);
	static void Run();
};