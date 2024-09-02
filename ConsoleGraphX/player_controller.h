#pragma once
#include "position_component.h"
#include "vector3.h"
#include "transform.h"

namespace ConsoleGraphX 
{
	struct PlayerController : public ConsoleGraphX_Internal::PositionComponentBase
	{
	public:
		float m_moveSpeed;
		float m_jumpSpeed;
		float m_gravity;
		Vector3 m_velocity;


	public:
		PlayerController();
		PlayerController(Transform* transform);
		PlayerController(float moveSpeed, float jumpSpeed, float gravity, Transform* transform);

		void Clone(PlayerController* pController) const;
	};
};
