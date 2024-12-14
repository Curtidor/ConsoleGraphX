#include "scene_switcher.h"
#include "../ConsoleGraphX/input_system.h"
#include "../ConsoleGraphX/scene_system.h"


using namespace ConsoleGraphX;

SceneSwitcher::SceneSwitcher(): Script(nullptr)
{
}

SceneSwitcher::SceneSwitcher(Entity* owner): Script(owner)
{
}

SceneSwitcher::~SceneSwitcher()
{
}

void SceneSwitcher::Awake()
{
}

void SceneSwitcher::Update(float deltaTime)
{
	if (InputSystem::IsKeyPressed(Key::Q))
	{
		_m_sceneSystem->LoadScene("Second Scene");
	}
}

void SceneSwitcher::AttachSceneSystem(SceneSystem* system)
{
	// aaahhh im sure the pointer isn't nullptr
	_m_sceneSystem = system;
}
