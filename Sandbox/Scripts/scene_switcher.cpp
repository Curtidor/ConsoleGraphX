#include "scene_switcher.h"
#include "../ConsoleGraphX/Engine/Systems/input_system.h"
#include "../ConsoleGraphX/Engine/Systems/scene_system.h"
#include "../ConsoleGraphX/Engine/Core/Logger/logger_manager.h"


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
	/*InputSystem& inputSystem = InputSystem::Instance();
	if (inputSystem.IsKeyPressed(Key::E))
	{
		_m_sceneSystem->LoadScene("Second Scene");
	}*/
}

void SceneSwitcher::AttachSceneSystem(SceneSystem* system)
{
	// aaahhh im sure the pointer isn't nullptr
	_m_sceneSystem = system;
}
