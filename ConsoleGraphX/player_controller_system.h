#pragma once
#include <unordered_map>
#include "base_system.h"
#include "entity.h"
#include "player_controller.h"

namespace ConsoleGraphX
{

	class PlayerControllerSystem : public BaseSystem
	{
	private:
		static std::unordered_map<Entity*, PlayerController*> _s_controllerPairs;

	public:
		void Initialize()  override;
		void Update(float delta_time) override;

		static void RegisterController(Entity* owner);
		static void DeregisterController(Entity* owner);
	};
};
