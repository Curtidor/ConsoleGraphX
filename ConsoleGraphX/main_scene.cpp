#include "main_scene.h"
#include <windows.h>
#include <string>
#include "scene_system.h"
#include "sprite.h"
#include "screen.h"
#include "entity.h"
#include "player.h"
#include "scene.h"
#include "snow.h"
#include "test_script.h"
#include "vector3.h"
#include "camera.h"
#include "camera_system.h"


MainScene::MainScene(std::string name) : Scene(name)
{
	SceneSystem::RegisterScene(this);
	SceneSystem::LoadScene(this->GetSceneName());
}

void MainScene::Initialize()
{
	//Entity* snowPrefab = this->RegisterEntityN("snow");
	//Sprite* snowSprite = (Sprite*)snowPrefab->AddComponent<Sprite>(2, 1, Color::White);
	//snowPrefab->GetComponent<Transform>()->SetPosition(Screen::GetWidth_A() / 2, 0);
	//Snow* snowScript = (Snow*)snowPrefab->AddComponent<Snow>();
	//snowScript->SetState(false);
	//

	//snowSprite->m_layer = 1;

	//Vector3 minSpread = Vector3(-135, 0,0);
	//Vector3 maxSpread = Vector3(135, 90, 0);

	//for (int i = 0; i < 2000; i++)
	//{
	//	// snowPrefab->CloneEntity(minSpread, maxSpread);
	//}


	Entity* grass = this->RegisterEntityN("Grass");
	grass->GetComponent<Transform>()->SetPosition(0, 12);
	Sprite* grassSprite = (Sprite*)grass->AddComponent<Sprite>(140, 10, Green);
	grassSprite->m_layer = 1;

	/*Entity* marker = this->RegisterEntityN("marker");
	marker->GetComponent<Transform>()->SetPosition(100, 0);
	Sprite* makerSprite = (Sprite*)marker->AddComponent<Sprite>(10, 10, Blue);
	makerSprite->m_layer = 1;*/

	
	/*Entity* sky = this->RegisterEntityN("Sky");
	sky->GetComponent<Transform>()->SetPosition(0, 0);
	Sprite* spriteSky = (Sprite*)sky->AddComponent<Sprite>(Screen::GetWidth_A(), 90, Blue);
	spriteSky->m_layer = 1;*/

	Entity* player = this->RegisterEntityN("Player");
	player->AddComponent<Player>();
	player->AddComponent<TestScript>();

	Vector3 playerPosition = player->GetPosition();

	Camera* cam = (Camera*)player->AddComponent<Camera>(120, 65, Vector3());
	CameraSystem::RegisterCamera(cam);
	CameraSystem::SetCameraState(cam, true);
	
	// console graph-x pixels
	/*CHAR_INFO* consoleGraphXPixels = new CHAR_INFO[595]{ {Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{Screen::s_transparentPixel, 0} };

	Entity* tE = new Entity();
	Sprite* spritetE1 = (Sprite*) tE->AddComponent<Sprite>(119, 5, consoleGraphXPixels);
	spritetE1->m_layer = 1;
	tE->GetComponent<Transform>()->SetPosition(Screen::GetWidth_A() / 2 - (119 / 2), 10);


	this->RegisterEntity(tE);

	Entity* wall = this->RegisterEntityN("wooden wall");
	wall->AddComponent<Sprite>(5, 30, Color::Yellow + Color::Gray);
	wall->GetComponent<Transform>()->SetPosition(12, 60);*/
	
}
