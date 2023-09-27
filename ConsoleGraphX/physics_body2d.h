#pragma once
#include "vector3.h"
#include "component.h"

struct PhysicsBody2D :  Component
{
private:
    Vector3 _m_velocity;
    Vector3 _m_acc;
    float _m_mass;
    float _m_friction;
    bool _m_gravity;
    bool _m_is_static;

public:
    PhysicsBody2D();
    PhysicsBody2D(bool gravity);

    int GetID() const override;

    Vector3 GetVelocity() const;
    void SetVelocity(const Vector3& velocity);

    Vector3 GetAcceleration() const;
    void SetAcceleration(const Vector3& acceleration);

    float GetMass() const;
    void SetMass(float mass);

    float GetFriction() const;
    void SetFriction(float friction);

    bool IsGravityEnabled() const;
    void SetGravityEnabled(bool gravity);
};
