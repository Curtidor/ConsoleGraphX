#include "player_controller_system.h"
#include <unordered_map>
#include <utility>
#include "dispatcher.h"
#include "entity.h"
#include "player_controller.h"
#include "input_system.h"
#include "transform.h"
#include "vector3.h"

std::unordered_map<Entity*, PlayerController*> PlayerControllerSystem::_s_controllerPairs;

void PlayerControllerSystem::Initialize() const 
{
	Dispatcher<Entity*>::RegisterListener("AddComponentstruct PlayerController", RegisterController);
	Dispatcher<Entity*>::RegisterListener("RemoveComponentstruct PlayerController", DeregisterController);
}

void PlayerControllerSystem::Update(float delta_time) const
{
	for (std::pair<Entity*, PlayerController*> controllerPair : _s_controllerPairs)
	{
        PlayerController* controller = controllerPair.second;
        Entity* owner = controllerPair.first;

        Transform* transform = owner->GetComponent<Transform>();

        controller->m_velocity.y -= controller->m_gravity * delta_time;

        Vector3 movement;

        if (InputSystem::IsKeyPressed(Key::A))
        {
            movement += Vector3::left;
        }
        else if (InputSystem::IsKeyPressed(Key::D))
        {
            movement += Vector3::right;
        }

        if (InputSystem::IsKeyPressed(Key::W))
        {
            movement += Vector3::down;
        }
        else if (InputSystem::IsKeyPressed(Key::S))
        {
            movement += Vector3::up;
        }

        transform->Translate(movement * controller->m_moveSpeed * delta_time);

	}
}

void PlayerControllerSystem::RegisterController(Entity* owner)
{
    PlayerController* controller = owner->GetComponent<PlayerController>();

    if (controller == nullptr)
        return;

    _s_controllerPairs.insert(std::make_pair(owner, controller));
}

void PlayerControllerSystem::DeregisterController(Entity* owner)
{
    if (owner == nullptr)
        return;

	PlayerController* controller = owner->GetComponent<PlayerController>();
	auto it = _s_controllerPairs.find(owner);

	if (it != _s_controllerPairs.end())
	{
		_s_controllerPairs.erase(it);
	}
}
