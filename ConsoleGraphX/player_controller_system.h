#pragma once
#include "base_system.h"

namespace ConsoleGraphX
{

	class PlayerControllerSystem : public BaseSystem
	{
	private:

	public:
		void Initialize()  override;
		void Update(float delta_time) override;
	};
};
