#pragma once
#include "player.h"
#include "../ConsoleGraphX/Engine/Core/entity.h"
#include "../ConsoleGraphX/Engine/Components/PositionComponents/player_controller.h"
#include "../ConsoleGraphX/Engine/Components/script.h"
#include "../ConsoleGraphX/Engine/Components/PositionComponents/sprite.h"
#include "../ConsoleGraphX/Engine/Components/transform.h"
#include "../ConsoleGraphX/Engine/Systems/input_system.h"
#include "../ConsoleGraphX/Engine/Components/camera.h"
#include "../ConsoleGraphX/Engine/Math/vector3.h"
#include "../ConsoleGraphX/Engine/Resources/Pools/base_resource_pool.h"

using namespace ConsoleGraphX;

Player::Player() : Script(nullptr), _m_playerCam(nullptr)
{}

Player::Player(Entity* owner) : Script(owner), _m_playerCam(nullptr)
{}

Player::~Player()
{}

void Player::Awake()
{
	// TODO: look into what we want the behavior to be should we expose the entities resource manager with a getter and use that manager
	// or use the scenes manager which may or may not be the same as the entities
	ConsoleGraphX_Internal::ResourceIndex index = _m_owner->GetResourceManager().CreateTextureResource("Sprites/player_sprite.cxsp").second;

	_m_playerCam = _m_owner->GetComponent<Camera>();

	_m_owner->AddComponent<Sprite>(index);

	Transform* transform = _m_owner->GetTransform();
	Sprite* sprite = _m_owner->GetComponent<Sprite>();

	_m_owner->GetComponent<Transform>()->SetPosition(50, 50);
	_m_owner->AddComponent<PlayerController>(100, 5, 9);
}

void Player::Update(float deltaTime)
{
	Vector3 playerPosition = _m_owner->GetComponent<Transform>()->m_position;

	/*InputSystem& i = InputSystem::Instance();
	if (i.IsKeyPressed(Key::B))
	{
		_m_owner->KillEntity();

	}*/
}


