#include <limits>
#include "transform.h"
#include "vector3.h"
#include "resource_manager.h"

namespace ConsoleGraphX
{
    Transform::Transform()
        : m_position(0.0f, 0.0f, 0.0f), m_previousPosition(0.0f, 0.0f, 0.0f),
        m_scale(1.0f, 1.0f, 1.0f), m_rotation(0.0f), m_parent((std::numeric_limits<TransformID>::max)())
    {}

    Transform::Transform(const Transform& other)
        : m_position(Vector3(other.m_position)), m_previousPosition(Vector3(other.m_previousPosition)),
        m_scale(Vector3(other.m_scale)), m_rotation(other.m_rotation), m_parent(other.m_parent)
    {}

    Transform::Transform(float x, float y, float scaleX, float scaleY)
        : m_position(x, y, 0.0f), m_previousPosition(x, y, 0.0f),
        m_scale(scaleX, scaleY, 1.0f), m_rotation(0.0f), m_parent((std::numeric_limits<TransformID>::max)())
    {}

    Transform::Transform(float x, float y, float z, float scaleX, float scaleY, float scaleZ)
        : m_position(x, y, z), m_previousPosition(x, y, z),
        m_scale(scaleX, scaleY, scaleZ), m_rotation(0.0f), m_parent((std::numeric_limits<TransformID>::max)())
    {}

    void Transform::SetParent(TransformID parent)
    {
        m_parent = parent;
    }

    Transform* Transform::GetParent() const
    {
        if (m_parent != (std::numeric_limits<TransformID>::max)())
        {
            return ConsoleGraphX_Internal::ResourceManager::GetActiveResourceManager().GetResource<ConsoleGraphX::Transform>(m_parent);
        }

        return nullptr;
    }

    void Transform::Clone(Transform* transform) const
    {
        *transform = *this;
    }

    void Transform::SetPosition(float x, float y)
    {
        m_previousPosition = m_position; // Track previous position
        m_position.x = x;
        m_position.y = y;
    }

    void Transform::SetPosition(float x, float y, float z)
    {
        m_previousPosition = m_position; // Track previous position
        m_position.x = x;
        m_position.y = y;
        m_position.z = z;
    }

    void Transform::SetPosition(const Vector3& position)
    {
        m_previousPosition = m_position; // Track previous position
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
        m_previousPosition = m_position; // Track previous position
        m_position += translation;
    }

    const Vector3& Transform::GetLocalPosition() const
    {
        return m_position;
    }

    const Vector3& Transform::GetPreviousPosition() const
    {
        return m_previousPosition; // Return the previous position
    }

    const Vector3 Transform::GetWorldPosition() const
    {
        if (m_parent != -1)
        {
            return m_position + GetParent()->GetWorldPosition();
        }

        return m_position;
    }
}
