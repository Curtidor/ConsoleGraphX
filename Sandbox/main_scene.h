#pragma once
#include <string>
#include "main_scene.h"
#include "./Scripts/snow.h"
#include "./Scripts/player.h"
#include "./Scripts/scene_switcher.h"
#include "./Engine/Scene/scene.h"
#include "./Engine/Components/PositionComponents/sprite.h"
#include "./Engine/Core/entity.h"
#include "./Engine/Scene/scene.h"
#include "./Engine/Math/vector3.h"
#include "./Engine/Components/camera.h"
#include "./Engine/Graphics/ScreenGraphics/screen.h"
#include "./Engine/Resources/map_loader.h"
// Register->Load->Initialize->Run

using namespace ConsoleGraphX;
using namespace ConsoleGraphX_Internal;

class MainScene : public Scene
{
public:

	MainScene(std::string name) : Scene(name)
	{}

	void Initialize() override
	{
		std::vector<ConsoleGraphX_Internal::Chunk> map_data = LoadMap("test.cxmap");

		for (ConsoleGraphX_Internal::Chunk& chunk : map_data)
		{
			for (auto& sprite_ids : chunk.m_sprites)
			{

			}
		}

		//TODO GET CAMERA SIZE
		Entity* camera = RegisterEntityN();
		camera->AddComponent<Camera>(300, 120, Vector3(0, 0));

		Entity* player = RegisterEntityN();
		player->AddComponent<Player>();

		Entity* snow = RegisterEntityN();
		snow->AddComponent<Snow>();
		snow->AddComponent<Sprite>(1, 1, 13);

		Entity* sceneSwitcher = RegisterEntityN();
		ResourceIndex switcherIndex = sceneSwitcher->AddComponent<SceneSwitcher>();

	/*	for (int i = 0; i < 700; i++)
		{
			Entity* clonedSnow = RegisterEntityN();

			snow->Clone(*clonedSnow, Vector3(0, 0, 0), Vector3(0, 0, 0));
		}*/


	}
};