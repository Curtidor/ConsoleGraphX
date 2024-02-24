#pragma once
#include "player_controller.h"
#include "sprite.h"
#include "script.h"


struct Player : public Script
{
public:
	Player(Entity* entity);

	void Awake() override;
	void Update() override;
};