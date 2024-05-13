#include "player_controller.h"
#include "component.h"

namespace ConsoleGraphX
{
    PlayerController::PlayerController() : m_moveSpeed(0), m_jumpSpeed(0), m_gravity(0), m_velocity(0.0f)
    {}

    PlayerController::PlayerController(float moveSpeed, float jumpSpeed, float gravity) :
        m_moveSpeed(moveSpeed), m_jumpSpeed(jumpSpeed), m_gravity(gravity), m_velocity(0.0f)
    {}

    int PlayerController::GetID() const
    {
        return ComponentID::playerController;
    }

    ConsoleGraphX_Interal::Component* PlayerController::Clone() const
    {
        PlayerController* clone = new PlayerController(*this);

        return clone;
    }
};