#pragma once
#include <unordered_map>
#include "system.h"
#include "dispatcher.h"
#include "player_controller.h"


class PlayerControllerSystem: public System
{
private:
	static std::unordered_map<Entity*, PlayerController*> _s_controllerPairs;

public:
	void Initialize() const override;
	void Update(float delta_time) const override;

	static void RegisterController(Entity* owner);
	static void DeregisterController(Entity* owner);
};