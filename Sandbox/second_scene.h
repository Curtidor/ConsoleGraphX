#pragma once
#include <string>
#include "main_scene.h"
#include "./Scripts/player.h"
#include "./Engine/Scene/scene.h"
#include "./Engine/Components/PositionComponents/sprite.h"
#include "./Engine/Core/entity.h"
#include "./Engine/Scene/scene.h"
#include "./Engine/Math/vector3.h"
#include "./Engine/Components/camera.h"
#include "./Engine/Graphics/ScreenGraphics/screen.h"

// Register->Load->Initialize->Run

using namespace ConsoleGraphX;
using namespace ConsoleGraphX_Internal;

class SecondScene : public Scene
{
public:

	SecondScene() : Scene("Second Scene")
	{}

	void Initialize() override
	{
		//TODO GET CAMERA SIZE
		Entity* camera = RegisterEntityN();
		camera->AddComponent<Camera>(300, 120, Vector3(0, 0));

		Entity* player = RegisterEntityN();
		player->AddComponent<Player>();

		ResourceIndex wallTIndex = _m_resourceManager.CreateTextureResource("Sprites/wall_sprite.cxsp").second;
		ResourceIndex grassTIndex = _m_resourceManager.CreateTextureResource("Sprites/grass_sprite.cxsp").second;

		Entity* wallL = RegisterEntityN();
		wallL->AddComponent<Sprite>(wallTIndex);
		wallL->GetTransform()->SetPosition(40.0f, static_cast<float>(120 - 40 - 12));

		Entity* wallR = RegisterEntityN();
		wallR->AddComponent<Sprite>(wallTIndex);
		wallR->GetTransform()->SetPosition(270.0f, static_cast<float>(120 - 40 - 12));

		Entity* grass = RegisterEntityN();
		grass->AddComponent<Sprite>(grassTIndex);
		grass->GetTransform()->SetPosition(0.0f, static_cast<float>(120 - 12));
	}
};