#include "player_controller.h"

PlayerController::PlayerController(Entity* owner, float move_speed, float jump_speed, float gravity) :
    m_owner(owner), m_move_speed(move_speed), m_jump_speed(jump_speed), m_gravity(gravity), m_velocity(0.0f)
{}

void PlayerController::Update(float delta_time)
{
    // Check for player input and update the player's position or perform actions.

    // Apply gravity to the player's velocity
    m_velocity.y -= m_gravity * delta_time;

    if (InputSystem::IsKeyPressed(Key::A))
    {
        m_owner->GetComponent<Transform>()->Translate(Vector3::left * m_move_speed * delta_time);
    }
    else if (InputSystem::IsKeyPressed(Key::D))
    {
        m_owner->GetComponent<Transform>()->Translate(Vector3::right * m_move_speed * delta_time);
    }

    if (InputSystem::IsKeyPressed(Key::Space))
    {
        // Check if the player is grounded (e.g., on the floor) before allowing a jump
        if (IsGrounded())
        {
            // Set the upward velocity for the jump
            m_velocity.y = m_jump_speed;
        }
    }

    // Apply the velocity to the player's position
   // m_owner->GetComponent<Transform>()->Translate(m_velocity * delta_time);
}

bool PlayerController::IsGrounded()
{
    // Check if the player's position is at or near the ground (you can adjust this threshold)
    // For example, you can check if the player's Y position is less than a certain value
    return m_owner->GetComponent<Transform>()->GetPosition().y <= 68.0f;
}
