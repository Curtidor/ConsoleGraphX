#include "box_collider2d.h"

BoxCollider2D::BoxCollider2D() 
    : size(Vector2(1, 1))
{}

BoxCollider2D::BoxCollider2D(const Vector2 & size) : size(size)
{}

bool BoxCollider2D::Intersects(Entity& a, Entity& b) {
    // Get the Transform components of both entities
    Transform* transformA = a.GetComponent<Transform>();
    Transform* transformB = b.GetComponent<Transform>();

    // Calculate the AABB (Axis-Aligned Bounding Box) of both colliders
    Vector3 positionA = transformA->GetPosition();
    Vector2 sizeA = a.GetComponent<BoxCollider2D>()->size;

    Vector3 positionB = transformB->GetPosition();
    Vector2 sizeB = b.GetComponent<BoxCollider2D>()->size;

    float minX1 = positionA.x;
    float maxX1 = positionA.x + sizeA.x;
    float minY1 = positionA.y;
    float maxY1 = positionA.y + sizeA.y;

    float minX2 = positionB.x;
    float maxX2 = positionB.x + sizeB.x;
    float minY2 = positionB.y;
    float maxY2 = positionB.y + sizeB.y;

    // Check for overlap along both the X and Y axes
    bool xOverlap = (minX1 <= maxX2 && maxX1 >= minX2);
    bool yOverlap = (minY1 <= maxY2 && maxY1 >= minY2);

    // If there is overlap along both axes, the colliders intersect
    return xOverlap && yOverlap;
}

