#pragma once
#include "collider2d.h"

struct CircleCollider2D : Collider2D
{
	float m_radius;

	CircleCollider2D();
	CircleCollider2D(float radius);
};