#pragma once
#include "collider2d.h"

struct BoxCollider2D: public Collider2D
{

    BoxCollider2D();
    BoxCollider2D(const Vector2& size);

    ~BoxCollider2D();

    const std::vector<Vector2*> GetVertices();
};
