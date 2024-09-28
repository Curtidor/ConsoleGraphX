#include <string>
#include "main_scene.h"
#include "screen.h"
#include "scene_system.h"
#include "sprite.h"
#include "entity.h"
#include "player.h"
#include "scene.h"
#include "snow.h"
#include "vector3.h"
#include "camera.h"
#include "color.h"
#include "resourcec_manager.h"
#include "base_resource_pool.h"

using namespace ConsoleGraphX;
using namespace ConsoleGraphX_Internal;

MainScene::MainScene(std::string name) : Scene(name)
{
	SceneSystem::RegisterScene(this);
	SceneSystem::LoadScene(GetSceneName());
}

void MainScene::Initialize()
{
	Entity* camera = RegisterEntityN();
	camera->AddComponent<Camera>(ConsoleGraphX_Internal::Screen::GetWidth_A(), ConsoleGraphX_Internal::Screen::GetHeight_A(), Vector3(0, 0));

	Entity* player = RegisterEntityN();
	player->AddComponent<Player>();

	Entity* snow = RegisterEntityN();
	snow->AddComponent<Snow>();
	snow->AddComponent<Sprite>(1,1, Color::Magenta);

	std::vector<Entity*> entities;
	for (int i = 0; i < 400; i++)
	{
		Entity* clonedSnow = RegisterEntityN();

		snow->Clone(*clonedSnow, Vector3(0, 0, 0), Vector3(0, 0, 0));
		entities.push_back(clonedSnow);
	}

	for (int i = 0; i < entities.size(); i++)
	{
		//entities[i]->DestroyEntity();
	}

	ConsoleGraphX_Internal::ResourceManager rManager = ConsoleGraphX_Internal::ResourceManager::Instance();
	auto* sprites = rManager.GetResourcePoolFromId(4);
	auto* scripts = rManager.GetResourcePoolFromId(0);

	ResourcePool<Transform>* tPool = static_cast<ResourcePool<Transform>*>(rManager.GetResourcePoolFromId(1));

	ResourceIndex wallTIndex = rManager.CreateTextureResource("../wall_sprite.cxsp").second;
	ResourceIndex grassTIndex = rManager.CreateTextureResource("../grass_sprite.cxsp").second;

	//tPool->Compress();

	Entity* wallL = RegisterEntityN();
	wallL->AddComponent<Sprite>(wallTIndex);
	wallL->GetTransform()->SetPosition(15, Screen::GetHeight_A() - 40-12);
	
	Entity* wallR = RegisterEntityN();
	wallR->AddComponent<Sprite>(wallTIndex);
	wallR->GetTransform()->SetPosition(270, Screen::GetHeight_A() - 40-12);

	Entity* grass = RegisterEntityN();
	grass->AddComponent<Sprite>(grassTIndex);
	grass->GetTransform()->SetPosition(0, Screen::GetHeight_A() - 12);



}
