#include "box_collider2d.h"

BoxCollider2D::BoxCollider2D() 
{
    this->vertices.push_back(new Vector2(0, 0));
    this->vertices.push_back(new Vector2(0, 1));
    this->vertices.push_back(new Vector2(1, 0));
    this->vertices.push_back(new Vector2(1, 1));
}

BoxCollider2D::BoxCollider2D(const Vector2& size) 
{
    this->vertices.push_back(new Vector2(0, 0));
    this->vertices.push_back(new Vector2(0, size.y));
    this->vertices.push_back(new Vector2(size.x, 0));
    this->vertices.push_back(new Vector2(size.x, size.y));
}

BoxCollider2D::~BoxCollider2D()
{
    this->vertices.clear();
}

const std::vector<Vector2*> BoxCollider2D::GetVertices()
{
    return this->vertices;
}

