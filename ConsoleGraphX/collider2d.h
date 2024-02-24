#pragma once
#include "vector2.h"
#include <vector>
#include "component.h"

//base class for 2d collides 
struct Collider2D : Component
{
protected:
	std::vector<Vector2*> vertices;

public:
	Collider2D();

	virtual const std::vector<Vector2*> GetVertices();
};