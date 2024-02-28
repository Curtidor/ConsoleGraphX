#pragma once
#include "entity.h"
#include "component.h"
#include "input_system.h"

struct PlayerController: Component
{
public:
	float m_moveSpeed;
	float m_jumpSpeed;
	float m_gravity;
	Vector3 m_velocity;

public:
	PlayerController();
	PlayerController(float moveSpeed, float jumpSpeed, float gravity);

	int GetID() const override;
};