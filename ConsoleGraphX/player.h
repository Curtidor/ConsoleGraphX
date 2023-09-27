#pragma once
#include "player_controller.h"
#include "script.h"


struct Player : public Script
{
private:
	

public:
	Player(Entity* entity);

	void Awake() override;
	bool Update() override;
};