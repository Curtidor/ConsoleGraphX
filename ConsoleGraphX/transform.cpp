#include "transform.h"
#include "vector3.h"

namespace ConsoleGraphX
{
    Transform::Transform()
        : m_position(0.0f, 0.0f, 0.0f), 
        m_scale(1.0f, 1.0f, 1.0f), m_rotation(0.0f), m_parent(nullptr)
    {}

    Transform::Transform(const Transform& transform)
        : m_position(Vector3(transform.m_position)), 
        m_scale(Vector3(transform.m_scale)), m_rotation(transform.m_rotation), m_parent(nullptr)
    {}

    Transform::Transform(float x, float y, float scaleX, float scaleY)
        : m_position(x, y, 0.0f), m_scale(scaleX, scaleY, 1.0f), m_rotation(0.0f), m_parent(nullptr)
    {}

    Transform::Transform(float x, float y, float z, float scaleX, float scaleY, float scaleZ)
        : m_position(x, y, z), m_scale(scaleX, scaleY, scaleZ), m_rotation(0.0f), m_parent(nullptr)
    {}

    void Transform::SetParent(Transform* parent)
    {
        if (parent == nullptr)
            return;

        m_parent = parent;
    }

    void Transform::Clone(Transform* transform) const
    {
        *transform = *this;
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

    void Transform::SetPosition(const Vector3& position)
    {
        m_position = position;
    }

    void Transform::SetScale(float x, float y)
    {
        SetScale(x, y, m_scale.z);
    }

    void Transform::SetScale(float x, float y, float z)
    {
        m_scale.x = x;
        m_scale.y = y;
        m_scale.z = z;
    }

    void Transform::Translate(const Vector3& translation)
    {
        m_position += translation;
    }

    const Vector3& Transform::GetLocalPosition() const
    {
        return m_position;
    }

    const Vector3& Transform::GetWorldPosition() const
    {
        if (m_parent != nullptr)
        {
            return m_position + m_parent->GetWorldPosition();
        }

        return m_position;
    }

    Transform* Transform::GetParentTransform() const 
    {
        return m_parent;
    }

};