#pragma once
#include <chrono>
#include <thread>
#include "render_system.h"
#include "sprtite_system.h"
#include "input_system.h"
#include "debugger.h"
#include "player_controller_system.h"
#include "physics_system.h"
#include "screen.h"
#include "scene_system.h"

class Engine
{
private:
	System* _m_system;
	Debugger* _m_debugger;

public:
	Engine(short screen_width, short screen_height, short pixel_width, short pixel_height, Debugger* debugger);
	void Run();
};