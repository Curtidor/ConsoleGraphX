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
	
	ComponentSystem::RegisterComponent<Sprite>();
	ComponentSystem::RegisterComponent<Transform>();

	Scene* scene = new Scene();
	SceneSystem::LoadScene(scene);

	Entity* entity = new Entity("Entity");

	entity->GetComponent<Transform>()->SetPosition(10, 10);
	entity->AddComponent<Sprite>(100, 10);

	scene->RegisterEntity(entity);

	engine.Run();
}


int main()
{
	StartGame();

	return 0;
}