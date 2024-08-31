﻿#include <string>
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
#include "camera_system.h"
#include "transform.h"

using namespace ConsoleGraphX;

MainScene::MainScene(std::string name) : Scene(name)
{
	SceneSystem::RegisterScene(this);
	SceneSystem::LoadScene(GetSceneName());
}

void MainScene::Initialize()
{
	const int sceneWidth = 100;
	const int sceneHeight = 100;
	const int camWidth = 300;
	const int camHeight = 120;

	Entity* snowPrefab = RegisterEntityN("snow");
	Sprite* snowSprite = (Sprite*)snowPrefab->AddComponent<Sprite>(2, 1, Color::White);
	snowPrefab->GetTransform()->SetPosition(-sceneWidth / 2, 0);
	Snow* snowScript = (Snow*)snowPrefab->AddComponent<Snow>();
	snowScript->SetState(false);

	snowSprite->m_layer = 21;

	Vector3 minSpread = Vector3(-135, 0,0);
	Vector3 maxSpread = Vector3(135, 90, 0);

	for (int i = 0; i < 80000; i++)
	{
		Entity* newEntity = RegisterEntityN();
		snowPrefab->Clone(*newEntity, minSpread, maxSpread);
	}


	Entity* grass = RegisterEntityN("Grass");
	grass->GetTransform()->SetPosition(-sceneWidth/2, sceneHeight-15);
	Sprite* grassSprite = (Sprite*)grass->AddComponent<Sprite>(sceneWidth, 10, Green);
	grassSprite->m_layer = 2;


	Entity* sky = RegisterEntityN("Sky");
	sky->GetTransform()->SetPosition(sceneWidth/2, 0);
	Sprite* spriteSky = (Sprite*)sky->AddComponent<Sprite>(sceneWidth, 90, Blue);
	spriteSky->m_layer = 40;

	Entity* player = RegisterEntityN("Player");
	player->AddComponent<Player>();
	Vector3 playerPosition = player->GetComponent<Transform>()->m_position;


	Camera* cam = (Camera*)player->AddComponent<Camera>(camWidth, camHeight, Vector3(-50, 0));
	CameraSystem::RegisterCamera(cam);
	CameraSystem::SetCameraState(cam, true);
	
	// console graph-x pixels
	CHAR_INFO* consoleGraphXPixels = new CHAR_INFO[595]{ {ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 12},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0},{L'█', 15},{L'█', 15},{ConsoleGraphX_Internal::Screen::s_transparentPixel, 0} };

	Entity* tE = RegisterEntityN();
	Sprite* spritetE1 = (Sprite*) tE->AddComponent<Sprite>(119, 5, consoleGraphXPixels);
	spritetE1->m_layer = 1;
	tE->GetComponent<Transform>()->SetPosition(ConsoleGraphX_Internal::Screen::GetWidth_A() / 2 - (119 / 2), 10);

	Entity* wall = RegisterEntityN("wooden wall");
	wall->AddComponent<Sprite>(5, 30, Color::Yellow + Color::Gray);
	wall->GetComponent<Transform>()->SetPosition(12, 60);
	
}
