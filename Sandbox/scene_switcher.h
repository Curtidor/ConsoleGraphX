#pragma once
#include "../ConsoleGraphX/script.h"
#include "../ConsoleGraphX/scene_system.h"

using namespace ConsoleGraphX;

struct SceneSwitcher : public Script
{
private:
	SceneSystem* _m_sceneSystem;

public:
	SceneSwitcher();
	SceneSwitcher(Entity* owner);
	~SceneSwitcher();

	void Awake() override;
	void Update(float deltaTime) override;


	void AttachSceneSystem(SceneSystem* system);
}; 
