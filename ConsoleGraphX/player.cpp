#include "player.h"
#include <string>
#include "entity.h"
#include "player_controller.h"
#include "script.h"
#include "sprite.h"
#include "transform.h"
#include "input_system.h"
#include "camera.h"
#include "vector3.h"
#include "debugger.h"

Player::Player() : Script(), _m_playerCam(nullptr)
{}

void Player::Awake(Entity* owner)
{
	Sprite* sprite = (Sprite*)owner->AddComponent<Sprite>(7, 12, DarkYellow);
	sprite->m_layer = 10;

	owner->GetComponent<Transform>()->SetPosition(0,75);
	owner->AddComponent<PlayerController>(25, 5, 9);

	_m_playerCam = owner->GetComponent<Camera>();

	Vector3 playerPosition = owner->GetPosition();
	Vector3 camPostion = _m_playerCam->GetPosition();

	_m_playerCam->SetPosition(Vector3(playerPosition.x - _m_playerCam->GetWidth() * 0.5f, playerPosition.y + _m_playerCam->GetHeight() * 0.5f));

}

void Player::Update(Entity* owner)
{
	Vector3 playerPosition = owner->GetPosition();
	Vector3 camPostion = _m_playerCam->GetPosition();

	if (InputSystem::IsKeyPressed(Key::B))
	{
		owner->KillEntity();
	}

	_m_playerCam->SetPosition(Vector3(playerPosition.x - _m_playerCam->GetWidth() * 0.5f, playerPosition.y + _m_playerCam->GetHeight() * 0.5f));

}


