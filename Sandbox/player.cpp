#pragma once
#include "player.h"
#include "../ConsoleGraphX/entity.h"
#include "../ConsoleGraphX/player_controller.h"
#include "../ConsoleGraphX/script.h"
#include "../ConsoleGraphX/sprite.h"
#include "../ConsoleGraphX/transform.h"
#include "../ConsoleGraphX/input_system.h"
#include "../ConsoleGraphX/camera.h"
#include "../ConsoleGraphX/vector3.h"
#include "../ConsoleGraphX/resource_manager.h"
#include "../ConsoleGraphX/base_resource_pool.h"

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
	ConsoleGraphX_Internal::ResourceIndex index = ConsoleGraphX_Internal::ResourceManager::GetActiveResourceManager().CreateTextureResource("../player_sprite.cxsp").second;

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

	if (InputSystem::IsKeyPressed(Key::B))
	{
		_m_owner->KillEntity();

	}
}


