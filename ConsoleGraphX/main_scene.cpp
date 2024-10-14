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
#include "base_resource_pool.h"

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
	camera->AddComponent<Camera>(ConsoleGraphX_Internal::Screen::GetWidth_A(), ConsoleGraphX_Internal::Screen::GetHeight_A(), Vector3(0, 0));

	Entity* player = RegisterEntityN();
	player->AddComponent<Player>();

	Entity* snow = RegisterEntityN();
	snow->AddComponent<Snow>();
	snow->AddComponent<Sprite>(1,1, 13);

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
	wallL->GetTransform()->SetPosition(15, Screen::GetHeight_A() - 40-12);
	
	Entity* wallR = RegisterEntityN();
	wallR->AddComponent<Sprite>(wallTIndex);
	wallR->GetTransform()->SetPosition(270, Screen::GetHeight_A() - 40-12);

	Entity* grass = RegisterEntityN();
	grass->AddComponent<Sprite>(grassTIndex);
	grass->GetTransform()->SetPosition(0, Screen::GetHeight_A() - 12);

	Entity* cool = RegisterEntityN();
 	//cool->AddComponent<Sprite>(TestTIndex);

}
