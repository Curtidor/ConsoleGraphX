#pragma once
#include "engine.h"
#include "player.h"

// Register->Load->Initialize->Run

class MainScene: public Scene
{
public:

	MainScene(std::string name);

	void Initalize() override;
};