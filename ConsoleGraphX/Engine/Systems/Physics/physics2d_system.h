#include "Engine/Systems/base_system.h"


namespace ConsoleGraphX
{
	class Physics2DSystem : BaseSystem
	{
		void Initialize() override;
		void Update(float delta_time, SceneSystem& sceneSystem) override;
		void ShutDown() override {};
	};
};

