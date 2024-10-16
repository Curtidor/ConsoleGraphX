#pragma once
#include "player_controller.h"
#include "sprite.h"
#include "script.h"


struct Player : public Script
{
public:
	Player();

	void Awake(Entity* owner) override;
	void Update(Entity* owner) override;
};