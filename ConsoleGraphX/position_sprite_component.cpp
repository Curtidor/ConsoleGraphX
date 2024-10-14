#include "position_sprite_component.h"
#include "transform.h"
#include "vector3.h"
#include "position_component.h"

namespace ConsoleGraphX_Internal
{
    PositionSpriteComponent::PositionSpriteComponent()
        : PositionComponentBase(), m_previousPosition(0, 0, 0), m_currentPosition(0, 0, 0)
    {
    }

    PositionSpriteComponent::PositionSpriteComponent(const ConsoleGraphX::TransformID transformID)
        : PositionComponentBase(transformID), m_previousPosition(0, 0, 0), m_currentPosition(0, 0, 0)
    {
    }

    ConsoleGraphX::Vector3 PositionSpriteComponent::GetPreviousPosition() const
    {
        return m_previousPosition;
    }

    ConsoleGraphX::Vector3 PositionSpriteComponent::GetCurrentPosition() const
    {
        return m_currentPosition;
    }

    void PositionSpriteComponent::SetPosition(const ConsoleGraphX::Vector3& newPosition)
    {
        m_previousPosition = m_currentPosition;
        m_currentPosition = newPosition;

        // Also update the transform's position if necessary
        ConsoleGraphX::Transform* transform = GetTransform();
        if (transform)
        {
            // should be setting the world positon
            transform->SetPosition(newPosition);
        }
    }
}
