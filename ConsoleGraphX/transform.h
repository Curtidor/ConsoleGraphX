#pragma once
#include "vector3.h"
#include "component.h"

struct Transform: Component
{
public:
    Vector3 m_position;
    Vector3 m_world_position;
    Vector3 m_scale;
    float m_rotation;

    Transform();
    Transform(float x, float y, float scaleX, float scaleY);
    Transform(float x, float y, float z, float scaleX, float scaleY, float scaleZ);
    ~Transform() override;

    void SetPosition(float x, float y);
    void SetPosition(float x, float y, float z);
    void SetScale(float x, float y);
    void SetScale(float x, float y, float z);

    const Vector3 GetPosition();
};
