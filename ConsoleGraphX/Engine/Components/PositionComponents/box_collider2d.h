#include "player_controller.h"
#include "../../Math/vector2.h"

namespace ConsoleGraphX
{
	struct CollisionPoint
	{
		Vector2 m_point; // The point of collision in world space
		Vector2 m_normal; // The normal of the collision
		float m_depth; // The depth of the collision
	};

	struct BoxCollider2D : public ConsoleGraphX_Internal::PositionComponentBase, ConsoleGraphX_Internal::Component
	{
		
	public:
		BoxCollider2D();
		BoxCollider2D(TransformID transform);


		CollisionPoint IsColliding(const BoxCollider2D& other) const;

	public:
		Vector2 m_size = Vector2(1.0f, 1.0f);
		Vector2 m_offset = Vector2(0.0f, 0.0f);
		bool m_isTrigger = false;
		bool m_isStatic = false;
		bool m_isEnabled = true;



	};
}