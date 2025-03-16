#pragma once
#include "../ConsoleGraphX/Engine/Components/script.h"
#include "../ConsoleGraphX/Engine/Components/camera.h"

using namespace ConsoleGraphX;

struct Player : public Script
{
private:
	Camera* _m_playerCam;

public:
	Player();
	Player(Entity* owner);
	~Player();

	void Awake() override;
	void Update(float deltaTime) override;
};