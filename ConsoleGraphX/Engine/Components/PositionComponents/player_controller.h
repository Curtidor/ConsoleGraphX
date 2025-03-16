#pragma once
#include "Engine\Components\PositionComponents\position_component.h"
#include "Engine\Math\vector3.h"
#include "Engine\Components\transform.h"
#include "Engine\Components\component.h"

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
