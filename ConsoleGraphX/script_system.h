#pragma once
#include <unordered_set>
#include "base_system.h"

namespace ConsoleGraphX
{
	class ScriptSystem : public BaseSystem
	{
	private:

	private:

	public:
		void Initialize() override;
		void Update(float delta_time, SceneSystem& sceneSystem) override;

		// I don't like this static, maybe if other system in the future need a warmup method well make it apart of the base
		// and do a system wide warmup
		static void ScriptWarmUp(SceneSystem& sceneSystem);
	};
};

