#include "main_scene.h"


MainScene::MainScene(std::string name) 
	: Scene(name)
{
	SceneSystem::RegisterScene(this);
	SceneSystem::LoadScene(this->GetSceneName());
}

void MainScene::Initialize()
{
	Entity* grass = new Entity("Grass");
	grass->GetComponent<Transform>()->SetPosition(0, 90);
	grass->AddComponent<Sprite>(Screen::GetWidth_A(), 10, Green);

	Entity* sky = new Entity("sky");
	sky->GetComponent<Transform>()->SetPosition(0, 0);
	sky->AddComponent<Sprite>(Screen::GetWidth_A(), 90, Blue);

	Entity* player = new Entity("Player");
	player->AddScript<Player>();

	CHAR_INFO* t = new CHAR_INFO[45]{ {L'█',Color::White},{L'█',Color::White},{L'█',Color::White},{L'█',Color::White},{L'█',Color::White},{L'█',Color::White},{L'█',Color::White},{L'█',Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{L'█',Color::White},{L'█',Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{L'█',Color::White},{L'█',Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{L'█',Color::White},{L'█',Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{L'█',Color::White},{L'█',Color::White} };
	CHAR_INFO* m = new CHAR_INFO[55]{ {L'█',Color::White},{L'█',Color::White},{L'█',Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{L'█',Color::White},{L'█',Color::White},{L'█',Color::White},{Screen::s_transparentPixel,Color::White},{L'█',Color::White},{L'█',Color::White},{L'█',Color::White},{L'█',Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{L'█',Color::White},{L'█',Color::White},{L'█',Color::White},{L'█',Color::White},{Screen::s_transparentPixel,Color::White},{L'█',Color::White},{L'█',Color::White},{Screen::s_transparentPixel,Color::White},{L'█',Color::White},{L'█',Color::White},{L'█',Color::White},{L'█',Color::White},{Screen::s_transparentPixel,Color::White},{L'█',Color::White},{L'█',Color::White},{Screen::s_transparentPixel,Color::White},{L'█',Color::White},{L'█',Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{L'█',Color::White},{L'█',Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{L'█',Color::White},{L'█',Color::White},{Screen::s_transparentPixel,Color::White},{L'█',Color::White},{L'█',Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{Screen::s_transparentPixel,Color::White},{L'█',Color::White},{L'█',Color::White} };

	Entity* mE = new Entity();
	mE->AddComponent<Sprite>(11, 5, m, true);
	mE->GetComponent<Transform>()->SetPosition(22, 10);


	Entity* tE = new Entity();
	tE->AddComponent<Sprite>(9, 5, t, true);
	tE->GetComponent<Transform>()->SetPosition(10, 10);
	

	this->RegisterEntity(mE);
	this->RegisterEntity(tE);
	this->RegisterEntity(grass);
	this->RegisterEntity(sky);
	this->RegisterEntity(player);
}
