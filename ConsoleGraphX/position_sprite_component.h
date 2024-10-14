#pragma once
#include "position_component.h"
#include "vector3.h"

namespace ConsoleGraphX_Internal
{
    class PositionSpriteComponent : public PositionComponentBase
    {
    public:
        PositionSpriteComponent();
        PositionSpriteComponent(const ConsoleGraphX::TransformID transformID);

        // Get the current and previous positions
        ConsoleGraphX::Vector3 GetPreviousPosition() const;
        ConsoleGraphX::Vector3 GetCurrentPosition() const;

        // Set the current position, automatically setting the previous position
        void SetPosition(const ConsoleGraphX::Vector3& newPosition);

    private:
        ConsoleGraphX::Vector3 m_previousPosition;
        ConsoleGraphX::Vector3 m_currentPosition;
    };
}
#pragma once
