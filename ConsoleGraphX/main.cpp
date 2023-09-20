#include <iostream>
#include "engine.h"
#include "scene_system.h"

/*
Fastest Font:
	Gothic: 800-900fps/avg
*/

void StartGame()
{
	Debugger debugger = Debugger(10);
	Engine engine = Engine(270, 100, 3, 6, &debugger);
	
	Scene* scene = new Scene();
	SceneSystem::LoadScene(scene);

	Entity* grass = new Entity("Grass");
	grass->GetComponent<Transform>()->SetPosition(0, 80);
	grass->AddComponent<Sprite>(270, 10, 10);
	
	Entity* dirt = new Entity("Dirt");
	dirt->GetComponent<Transform>()->SetPosition(0, 0);
	dirt->AddComponent<Sprite>(270, 80, 3);

	Entity* player = new Entity("Player");
	player->GetComponent<Transform>()->SetPosition(10, 68);
	player->AddComponent<Sprite>(7, 12, 6);
	player->AddComponent<PlayerController>(player, 50.0f, 5, 9);
	PhysicsBody2D* p_body = (PhysicsBody2D*)player->AddComponent<PhysicsBody2D>();


	scene->RegisterEntity(grass);
	scene->RegisterEntity(dirt);
	scene->RegisterEntity(player);

	p_body->SetAcceleration(Vector3(10, 0, 0));

	engine.Run();
}


int main()
{
	StartGame();

	return 0;
}