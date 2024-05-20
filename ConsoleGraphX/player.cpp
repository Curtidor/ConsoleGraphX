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

using namespace ConsoleGraphX;

Player::Player() : Script(), _m_playerCam(nullptr)
{}

void Player::Awake(Entity* owner)
{
	_m_playerCam = owner->GetComponent<Camera>();

	Sprite* sprite = (Sprite*)owner->AddComponent<Sprite>(7, 12, DarkYellow);
	sprite->m_layer = 10;

	owner->GetComponent<Transform>()->SetPosition(0,40);
	owner->AddComponent<PlayerController>(100, 5, 9);
}

void Player::Update(Entity* owner)
{
	Vector3 playerPosition = owner->GetPosition();
	Vector3 camPostion = _m_playerCam->GetPosition();

	_m_playerCam->SetPosition(Vector3(playerPosition.x - _m_playerCam->GetWidth() * 0.5f, playerPosition.y - _m_playerCam->GetHeight() * 0.5f));

	ConsoleGraphX_Interal::Debugger::S_LogMessage("player x: " + std::to_string(playerPosition.x) + " y: " + std::to_string(playerPosition.y));
	ConsoleGraphX_Interal::Debugger::S_LogMessage("cam x: " + std::to_string(camPostion.x) + " y: " + std::to_string(camPostion.y));

	if (InputSystem::IsKeyPressed(Key::B))
		owner->KillEntity();
}


