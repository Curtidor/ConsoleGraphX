#pragma once
#include <string>
#include "main_scene.h"
#include "snow.h"
#include "../ConsoleGraphX/scene.h"
#include "../ConsoleGraphX/scene_system.h"
#include "../ConsoleGraphX/sprite.h"
#include "../ConsoleGraphX/entity.h"
#include "../ConsoleGraphX/player.h"
#include "../ConsoleGraphX/scene.h"
#include "../ConsoleGraphX/vector3.h"
#include "../ConsoleGraphX/camera.h"
#include "../ConsoleGraphX/base_resource_pool.h"
#include "../ConsoleGraphX/screen.h"
#include "../ConsoleGraphX/window.h"
#include "../ConsoleGraphX/window_manager.h"
#include "window_script.h"

using namespace ConsoleGraphX;
using namespace ConsoleGraphX_Internal;

MainScene::MainScene(std::string name) : Scene(name)
{
	SceneSystem::Instance().RegisterScene(this);
	SceneSystem::Instance().LoadScene(GetSceneName());
}

void MainScene::Initialize()
{
	Entity* camera = RegisterEntityN();
	camera->AddComponent<Camera>(Screen::GetWidth_A(), Screen::GetHeight_A(), Vector3(0, 0));

	Entity* player = RegisterEntityN();
	player->AddComponent<Player>();

	Entity* snow = RegisterEntityN();
	snow->AddComponent<Snow>();
	snow->AddComponent<Sprite>(1, 1, 13);

	for (int i = 0; i < 700; i++)
	{
		Entity* clonedSnow = RegisterEntityN();

		snow->Clone(*clonedSnow, Vector3(0, 0, 0), Vector3(0, 0, 0));
	}

	ResourceIndex wallTIndex = _m_resourceManager.CreateTextureResource("../wall_sprite.cxsp").second;
	ResourceIndex grassTIndex = _m_resourceManager.CreateTextureResource("../grass_sprite.cxsp").second;
	ResourceIndex TestTIndex = _m_resourceManager.CreateTextureResource("../shoot_game.cxsp").second;

	Entity* wallL = RegisterEntityN();
	wallL->AddComponent<Sprite>(wallTIndex);
	wallL->GetTransform()->SetPosition(15, static_cast<float>(Screen::GetHeight_A() - 40 - 12));

	Entity* wallR = RegisterEntityN();
	wallR->AddComponent<Sprite>(wallTIndex);
	wallR->GetTransform()->SetPosition(270, static_cast<float>(Screen::GetHeight_A() - 40 - 12));

	Entity* grass = RegisterEntityN();
	grass->AddComponent<Sprite>(grassTIndex);
	grass->GetTransform()->SetPosition(0, static_cast<float>(Screen::GetHeight_A() - 12));

	Entity* cool = RegisterEntityN();
	cool->AddComponent<TestWindowScript>();
}
