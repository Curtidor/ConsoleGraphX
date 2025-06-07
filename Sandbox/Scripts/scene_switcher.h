#pragma once
#include "../ConsoleGraphX/Engine/Components/script.h"
#include "../ConsoleGraphX/Engine/Systems/scene_system.h"

using namespace ConsoleGraphX;

struct SceneSwitcher : public Script
{
public:
	SceneSwitcher();
	SceneSwitcher(Entity* owner);
	~SceneSwitcher();

	void Awake() override;
	void Update(UpdateContext& context) override;

}; 
