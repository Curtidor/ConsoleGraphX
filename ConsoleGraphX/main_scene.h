#pragma once
#include "engine.h"
#include "player.h"
#include "snow.h"
#include "gui_element.h"

// Register->Load->Initialize->Run

class MainScene: public Scene
{
public:

	MainScene(std::string name);

	void Initialize() override;
};