#pragma once
#include <string>
#include "scene.h"

// Register->Load->Initialize->Run

class MainScene: public Scene
{
public:

	MainScene(std::string name);

	void Initialize() override;
};