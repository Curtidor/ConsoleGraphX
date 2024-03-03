#pragma once
#include "script.h"
#include "entity.h"


struct Player : public Script
{
public:
	Player();

	void Awake(Entity* owner) override;
	void Update(Entity* owner) override;
};