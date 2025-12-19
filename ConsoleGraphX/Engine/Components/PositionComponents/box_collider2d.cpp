#include "PCH_CGX.h"
#include "box_collider2d.h"

ConsoleGraphX::BoxCollider2D::BoxCollider2D() : PositionComponentBase(), 
m_size(1.0f, 1.0f), m_offset(0.0f, 0.0f), m_isTrigger(false), m_isEnabled(true)
{
}

ConsoleGraphX::BoxCollider2D::BoxCollider2D(TransformID transform) : PositionComponentBase(transform),
m_size(1.0f, 1.0f), m_offset(0.0f, 0.0f), m_isTrigger(false), m_isEnabled(true)
{
}

ConsoleGraphX::CollisionPoint ConsoleGraphX::BoxCollider2D::IsColliding(const BoxCollider2D& other) const
{
	// check if the bounding boxes of the two colliders overlap
	if (!m_isEnabled || !other.m_isEnabled) 
	{
		return ConsoleGraphX::CollisionPoint(); // if either collider is disabled, they cannot collide
	}

	float thisLeft = m_offset.x - m_size.x / 2.0f;
	float thisRight = m_offset.x + m_size.x / 2.0f;
	float thisTop = m_offset.y - m_size.y / 2.0f;
	float thisBottom = m_offset.y + m_size.y / 2.0f;
	float otherLeft = other.m_offset.x - other.m_size.x / 2.0f;
	float otherRight = other.m_offset.x + other.m_size.x / 2.0f;
	float otherTop = other.m_offset.y - other.m_size.y / 2.0f;
	float otherBottom = other.m_offset.y + other.m_size.y / 2.0f;

	if (thisLeft < otherRight && thisRight > otherLeft && 
		thisTop < otherBottom && thisBottom > otherTop) 
	{
		CollisionPoint collisionPoint;
		collisionPoint.m_depth = std::min<float>(
			std::min<float>(thisRight - otherLeft, otherRight - thisLeft),
			std::min<float>(thisBottom - otherTop, otherBottom - thisTop)
		);
		collisionPoint.m_normal = { 0.0f, 0.0f };
		collisionPoint.m_point = {
			(thisLeft + thisRight) / 2.0f,
			(thisTop + thisBottom) / 2.0f
		};

		return collisionPoint;
		
	}

	return ConsoleGraphX::CollisionPoint();
}

