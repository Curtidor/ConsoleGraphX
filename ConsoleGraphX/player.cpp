#include "player.h"
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

Player::Player() : Script(nullptr), _m_playerCam(nullptr)
{}

Player::Player(Entity* owner) : Script(owner), _m_playerCam(nullptr)
{}

void Player::Awake()
{
	_m_playerCam = _m_owner->GetComponent<Camera>();

	_m_owner->AddComponent<Sprite>(7, 12, Cyan);

	Transform* transform = _m_owner->GetTransform();
	Sprite* sprite = _m_owner->GetComponent<Sprite>();

	_m_owner->GetComponent<Transform>()->SetPosition(50,50);
	_m_owner->AddComponent<PlayerController>(100, 5, 9);
}

void Player::Update(float deltaTime)
{
	Vector3 playerPosition = _m_owner->GetComponent<Transform>()->m_position;

	ConsoleGraphX_Internal::Debugger::S_LogMessage("player stuff");

	if (InputSystem::IsKeyPressed(Key::B))
	{
		_m_owner->KillEntity();
		ConsoleGraphX_Internal::Debugger::S_LogMessage("dead!");

	}
}


