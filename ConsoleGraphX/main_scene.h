#pragma once
#include <string>
#include "scene.h"

// Register->Load->Initialize->Run

using namespace ConsoleGraphX;

class MainScene: public Scene
{
public:

	MainScene(std::string name);

	void Initialize() override;
};