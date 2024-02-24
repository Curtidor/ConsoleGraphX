#include "player.h"

Player::Player(Entity* entity) 
	: Script(entity) 
{}

void Player::Awake()
{
	this->m_entity->AddComponent<Sprite>(7, 12, DarkYellow);
	this->m_entity->GetComponent<Transform>()->SetPosition(10, 78);
	this->m_entity->AddComponent<PlayerController>(this->m_entity, 25, 5, 9);

	this->m_is_enabled = true;
}

void Player::Update()
{
}



