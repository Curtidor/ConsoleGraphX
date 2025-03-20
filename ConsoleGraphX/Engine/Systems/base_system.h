#pragma once

namespace ConsoleGraphX
{
	class SceneSystem;

	class BaseSystem
	{
	public:
		virtual void Initialize() = 0;
		virtual void Update(float delta_time, SceneSystem& sceneSystem) = 0;

		virtual ~BaseSystem() = default;
	};
};
