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
#include "./Engine/Systems/scene_system.h"
#include "./Engine/Core/Application/application.h"


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
	ConsoleGraphX_Internal::ResourceIndex index = _m_owner->GetResourceManager().CreateTextureResource("Animations/player_animation_250.cxsp").second;

	_m_playerCam = _m_owner->GetComponent<Camera>();

	_m_owner->AddComponent<Sprite>(index);

	//Transform* transform = _m_owner->GetTransform();
	Sprite* sprite = _m_owner->GetComponent<Sprite>();

	_m_owner->GetComponent<Transform>()->SetPosition(50.0f, 50.0f);
	_m_owner->AddComponent<PlayerController>(100.0f, 5.0f, 9.0f);
}

void Player::Update(UpdateContext& context)
{
	Vector3 playerPosition = _m_owner->GetComponent<Transform>()->m_position;

	if (context.input.IsKeyPressed(Key::B))
	{
		_m_owner->KillEntity();
	}
#if MIN_BUILD == 1
	else if (context.input.IsKeyPressed(Key::K))
	{
		context.app->OnConsoleClose(nullptr);
	}
#endif
}


