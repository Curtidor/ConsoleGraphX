#pragma once
#include "entity.h"
#include "component.h"
#include "input_system.h"

struct PlayerController: Component
{
private:
	Entity* m_owner;
	float m_move_speed;
	float m_jump_speed;
	float m_gravity;
	Vector3 m_velocity;
public:
	PlayerController();
	PlayerController(Entity* owner, float move_speed, float jump_speed, float gravity);
	void Update(float delta_time);
	bool IsGrounded();
};