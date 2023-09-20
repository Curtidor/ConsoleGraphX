#pragma once
#include "collider2d.h"
#include "entity.h"
#include "vector2.h"

struct BoxCollider2D: Collider2D
{

    Vector2 size;   
    
    BoxCollider2D();
    BoxCollider2D(const Vector2& size);
    static bool Intersects(Entity& a, Entity& b) ;
};
