#include "window_script.h"
#include "../ConsoleGraphX/window_manager.h"


TestWindowScript::TestWindowScript() : _m_window(nullptr)
{}

TestWindowScript::TestWindowScript(Entity* entity) :  Script(entity), _m_window(nullptr)
{
}

void TestWindowScript::Awake()
{
	_m_window = WindowManager::Instance().CreateCGXWindow(200, 75, 3, 3, "TEST");
}

void TestWindowScript::Update(float deltaTime)
{
	static unsigned short color = 0;
	//_m_window->FillCanvas(CHAR_INFO{ ' ', color });

	//color++;

	if (color > 16)
		color = 0;
}
