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

void SceneSwitcher::Update(UpdateContext& context)
{
	if (context.input.IsKeyPressed(Key::E))
	{
		context.sceneSystem.LoadScene("Second Scene");
	}
}

