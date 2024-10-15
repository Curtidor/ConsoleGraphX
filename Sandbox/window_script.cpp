#include "window_script.h"
#include "../ConsoleGraphX/window_manager.h"


TestWindowScript::TestWindowScript()
{
}

TestWindowScript::TestWindowScript(Entity* entity) : Script(entity)
{
}

void TestWindowScript::Awake()
{
	_m_window = WindowManager::Instance().CreateCGXWindow(300, 170, 3, 3, "TEST WINDOW SCRIPT");
}

void TestWindowScript::Update(float deltaTime)
{
	static unsigned short color = 0;
	_m_window->Test(color);

	color++;

	if (color > 16)
		color = 0;
}
