#pragma once
#include "../ConsoleGraphX/script.h"
#include "../ConsoleGraphX/window.h"

using namespace ConsoleGraphX;

class TestWindowScript: public Script
{
public:
	TestWindowScript();
	TestWindowScript(Entity* entity);
	void Awake() override;
	void Update(float deltaTime) override;

private:
	Window* _m_window;
};