#include "player.h"

Player::Player(Entity* entity) 
	: Script(entity) 
{}

void Player::Awake()
{
	this->m_entity->AddComponent<Sprite>(7, 12, 6);
	this->m_entity->GetComponent<Transform>()->SetPosition(10, 78);
	this->m_entity->AddComponent<PlayerController>(this->m_entity, 50, 5, 9);
}

bool Player::Update()
{
	return true;
}



