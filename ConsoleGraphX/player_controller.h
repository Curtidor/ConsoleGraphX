#pragma once
#include "position_component.h"
#include "vector3.h"
#include "transform.h"
#include "component.h"

namespace ConsoleGraphX 
{
	struct PlayerController : public ConsoleGraphX_Internal::PositionComponentBase, ConsoleGraphX_Internal::Component
	{
	public:
		float m_moveSpeed;
		float m_jumpSpeed;
		float m_gravity;
		Vector3 m_velocity;


	public:
		PlayerController();
		PlayerController(TransformID transform);
		PlayerController(float moveSpeed, float jumpSpeed, float gravity, TransformID transform);

		void Clone(PlayerController* pController) const;
	};
};
