#include "physics_body2d.h"


PhysicsBody2D::PhysicsBody2D()
    : _m_velocity(Vector3::zero), _m_mass(1.0f), _m_friction(0.0f), _m_gravity(true), _m_is_static(false)
{
}


PhysicsBody2D::PhysicsBody2D( bool gravity)
    : _m_velocity(Vector3::zero), _m_mass(1.0f), _m_friction(0.0f), _m_gravity(gravity), _m_is_static(false)
{}

int PhysicsBody2D::GetID() const 
{
    return ComponentID::physics_body2d;
}

// Getter and setter functions for velocity
Vector3 PhysicsBody2D::GetVelocity() const
{
    return _m_velocity;
}

void PhysicsBody2D::SetVelocity(const Vector3& velocity)
{
    _m_velocity = velocity;
}

Vector3 PhysicsBody2D::GetAcceleration() const
{
    return _m_acc;
}

void PhysicsBody2D::SetAcceleration(const Vector3& acc)
{
    _m_acc = acc;
}

// Getter and setter functions for mass
float PhysicsBody2D::GetMass() const
{
    return _m_mass;
}

void PhysicsBody2D::SetMass(float mass)
{
    _m_mass = mass;
}

// Getter and setter functions for friction
float PhysicsBody2D::GetFriction() const
{
    return _m_friction;
}

void PhysicsBody2D::SetFriction(float friction)
{
    _m_friction = friction;
}

// Getter and setter functions for gravity
bool PhysicsBody2D::IsGravityEnabled() const
{
    return _m_gravity;
}

void PhysicsBody2D::SetGravityEnabled(bool gravity)
{
    _m_gravity = gravity;
}
