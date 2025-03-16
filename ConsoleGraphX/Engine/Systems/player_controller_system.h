#pragma once
#include "Engine\Systems\base_system.h"

namespace ConsoleGraphX
{

	class PlayerControllerSystem : public BaseSystem
	{
	private:

	public:
		void Initialize()  override;
		void Update(float delta_time, SceneSystem& sceneSystem) override;
	};
};
