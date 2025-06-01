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
#include "./Engine/Resources/texture_loader.h"
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
		const std::vector<ConsoleGraphX_Internal::Chunk> map_data = LoadMap("test.cxmap");
		const std::unordered_map<uint32_t, std::string> reg = LoadSpriteRegistry("C:/Users/tanja/OneDrive/Desktop/ConsoleGraphXFolder/Tools/WorldEditor/sprites.cxreg");

		for (const Chunk& chunk : map_data) {
			for (size_t i = 0; i < chunk.m_spriteCount; i++) {
				// lookup sprite file path from registry
				auto it = reg.find(chunk.m_sprites[i].m_spriteId);
				if (it == reg.end())
					continue;
					
				ResourceManager& rManager = GetResourceManager();
				// load texture -> get index
				const size_t textureIndexInPool = rManager.CreateTextureResource(it->second).second;

				// position sprite in world
				const Vector3& pos = chunk.m_sprites[i].m_mapPosition;
				const size_t transformIndexInPool = rManager.CreateResource<Transform>(pos.x, pos.y, pos.z, 1, 1, 1).second;

				// register sprite with transform + texture

				rManager.CreateResource<Sprite>(textureIndexInPool, &GetResourceManager(), transformIndexInPool);
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