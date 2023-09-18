#pragma once
#include <chrono>
#include <thread>
#include "render_system.h"
#include "sprtite_system.h"
#include "input_system.h"
#include "debugger.h"
#include "screen.h"

class Engine
{
private:
	std::vector<System*> m_systems;
	Debugger* m_debugger;

public:
	Engine(short screen_width, short screen_height, short pixel_width, short pixel_height, Debugger* debugger);
	void Run();
};