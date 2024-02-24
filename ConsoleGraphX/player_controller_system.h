#pragma once
#include <unordered_set>
#include "system.h"
#include "dispatcher.h"
#include "player_controller.h"


class PlayerControllerSystem: public System
{
private:
	static std::unordered_set<PlayerController*> _s_controllers;

public:
	void Initialize() const override;
	void Update(float delta_time) const override;

	static void RegisterController(Entity* owner);
	static void DeregisterController(Entity* owner);
};