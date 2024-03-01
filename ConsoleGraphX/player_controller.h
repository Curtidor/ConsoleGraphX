#pragma once
#include "component.h"
#include "vector3.h"
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
	
	Component* Clone() const override;
	int GetID() const override;
};