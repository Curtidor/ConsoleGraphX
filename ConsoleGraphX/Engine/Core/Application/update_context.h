#pragma once
#include "./Engine/Systems/input_system.h"

namespace ConsoleGraphX
{
	class SceneSystem; // sorry

	struct UpdateContext
	{
		SceneSystem& sceneSystem;
		float deltaTime;
		Input input;
	};
}
