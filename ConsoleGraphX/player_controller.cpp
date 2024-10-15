#include "CGXPCH.h"
#include "player_controller.h"
#include "position_component.h"
#include "transform.h"

namespace ConsoleGraphX
{
    PlayerController::PlayerController() : PositionComponentBase(), m_moveSpeed(0), m_jumpSpeed(0), m_gravity(0), m_velocity(0.0f)
    {
    }

    PlayerController::PlayerController(TransformID transformID) : PositionComponentBase(transformID),  m_moveSpeed(0), m_jumpSpeed(0), m_gravity(0), m_velocity(0.0f)
    {}

    PlayerController::PlayerController(float moveSpeed, float jumpSpeed, float gravity, TransformID transformID) :
        PositionComponentBase(transformID), m_moveSpeed(moveSpeed), m_jumpSpeed(jumpSpeed), m_gravity(gravity), m_velocity(0.0f)
    {}

    void PlayerController::Clone(PlayerController* pController) const
    {
        *pController = *this;
    }
};