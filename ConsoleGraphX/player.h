#pragma once
#include "script.h"
#include "entity.h"
#include "camera.h"


struct Player : public Script
{
private:
	Camera* _m_playerCam;

public:
	Player();

	void Awake(Entity* owner) override;
	void Update(Entity* owner) override;
};