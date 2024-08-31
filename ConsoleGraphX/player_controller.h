#pragma once
#include "transform.h"
#include "component.h"
#include "vector3.h"

namespace ConsoleGraphX 
{
	struct PlayerController : public ConsoleGraphX_Internal::Component
	{
	public:
		float m_moveSpeed;
		float m_jumpSpeed;
		float m_gravity;
		Vector3 m_velocity;
		Transform m_transform;


	public:
		PlayerController();
		PlayerController(float moveSpeed, float jumpSpeed, float gravity);

		void Clone(PlayerController* pController) const;
	};
};
