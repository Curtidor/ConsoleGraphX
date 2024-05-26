#pragma once
#include "component.h"
#include "vector3.h"

namespace ConsoleGraphX 
{
	struct PlayerController : ConsoleGraphX_Internal::Component
	{
	public:
		float m_moveSpeed;
		float m_jumpSpeed;
		float m_gravity;
		Vector3 m_velocity;

	public:
		PlayerController();
		PlayerController(float moveSpeed, float jumpSpeed, float gravity);

		ConsoleGraphX_Internal::Component* Clone() const override;
		int GetID() const override;
	};
};
