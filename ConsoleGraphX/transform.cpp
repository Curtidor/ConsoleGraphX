#include "transform.h"

Transform::Transform()
    : m_position(0.0f, 0.0f, 0.0f), m_world_position(0.0f, 0.0f, 0.0f), m_scale(1.0f, 1.0f, 1.0f), m_rotation(0.0f)
{
}

Transform::Transform(float x, float y, float scaleX, float scaleY)
    : m_position(x, y, 0.0f), m_world_position(x, y, 0.0f), m_scale(scaleX, scaleY, 1.0f), m_rotation(0.0f)
{
}

Transform::Transform(float x, float y, float z, float scaleX, float scaleY, float scaleZ)
    : m_position(x, y, z), m_world_position(x, y, z), m_scale(scaleX, scaleY, scaleZ), m_rotation(0.0f)
{
}

Transform::~Transform()
{
}

void Transform::SetPosition(float x, float y)
{
    m_position.x = x;
    m_position.y = y;
}

void Transform::SetPosition(float x, float y, float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
}

void Transform::SetScale(float x, float y)
{
    m_scale.x = x;
    m_scale.y = y;
}

void Transform::SetScale(float x, float y, float z)
{
    m_scale.x = x;
    m_scale.y = y;
    m_scale.z = z;
}

const Vector3 Transform::GetPosition()
{
    return m_position;
}

void Transform::Translate(const Vector3& translation)
{
    m_position += translation;
}
