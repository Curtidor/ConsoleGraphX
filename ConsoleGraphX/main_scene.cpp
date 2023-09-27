#include "main_scene.h"


MainScene::MainScene(std::string name) 
	: Scene(name)
{
	SceneSystem::RegisterScene(this);
	SceneSystem::LoadScene(this->GetSceneName());
}

void MainScene::Initalize()
{
	Entity* grass = new Entity("Grass");
	grass->GetComponent<Transform>()->SetPosition(0, 90);
	grass->AddComponent<Sprite>(270, 10, Green);

	Entity* sky = new Entity("sky");
	sky->GetComponent<Transform>()->SetPosition(0, 0);
	sky->AddComponent<Sprite>(270, 90, Blue);

	Entity* player = new Entity("Player");
	player->AddScript<Player>();


	this->RegisterEntity(grass);
	this->RegisterEntity(sky);
	this->RegisterEntity(player);
}