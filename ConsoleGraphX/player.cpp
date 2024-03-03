#include "player.h"
#include "entity.h"
#include "player_controller.h"
#include "script.h"
#include "sprite.h"
#include "transform.h"
#include "input_system.h"

Player::Player() : Script() 
{}

void Player::Awake(Entity* owner)
{
	Sprite* sprite = (Sprite*)owner->AddComponent<Sprite>(7, 12, DarkYellow);
	sprite->m_layer = 4;
	owner->GetComponent<Transform>()->SetPosition(22, 78);
	owner->AddComponent<PlayerController>(25, 5, 9);
}

void Player::Update(Entity* owner)
{
	if (InputSystem::IsKeyPressed(Key::B))
	{
		owner->KillEntity();
	}
}



