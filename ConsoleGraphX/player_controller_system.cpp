#include "player_controller_system.h"

std::unordered_set<PlayerController*> PlayerControllerSystem::_s_controllers;

void PlayerControllerSystem::Initalize()const 
{
	Dispatcher<Entity*>::RegisterListener("AddComponentstruct PlayerController", RegisterController);
	Dispatcher<Entity*>::RegisterListener("RemoveComponentstruct PlayerController", DeregisterController);
}

void PlayerControllerSystem::Update()const
{
	for (PlayerController* controller : _s_controllers)
	{
		controller->Update(0.0167f);
	}
}

void PlayerControllerSystem::RegisterController(Entity* owner)
{
	_s_controllers.insert(owner->GetComponent<PlayerController>());
}

void PlayerControllerSystem::DeregisterController(Entity* owner)
{
	PlayerController* controller = owner->GetComponent<PlayerController>();
	auto it = _s_controllers.find(controller);

	if (it != _s_controllers.end())
	{
		_s_controllers.erase(it);
	}
}
