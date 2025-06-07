#pragma once
#include "./Engine/Systems/input_system.h"
//#include "./Engine/Core/Application/application.h"


namespace ConsoleGraphX
{
#if MIN_BUILD == 1
	class Application;
#endif
	class SceneSystem; // sorry

	struct UpdateContext
	{
		SceneSystem& sceneSystem;
		float deltaTime;
		Input input;
	#if MIN_BUILD == 1
		Application* app;
	#endif
	
	};
}
