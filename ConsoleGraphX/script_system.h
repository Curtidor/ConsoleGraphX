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
		void Update(float delta_time) override;

		static void ScriptWarmUp();
	};
};

