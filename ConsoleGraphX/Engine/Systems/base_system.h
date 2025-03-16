#pragma once
#include "Engine\Systems\scene_system.h"

namespace ConsoleGraphX
{
	class BaseSystem
	{
	public:
		virtual void Initialize() = 0;
		virtual void Update(float delta_time, SceneSystem& sceneSystem) = 0;

		virtual ~BaseSystem() = default;
	};
};
