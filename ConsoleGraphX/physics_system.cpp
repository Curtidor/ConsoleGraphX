#include "physics_system.h"

std::unordered_set<Entity*> PhysicsSystem::_s_physics_bodies;
std::unordered_set<Entity*> PhysicsSystem::_s_colliders;

void PhysicsSystem::Initalize() const
{
    Dispatcher<Entity*>::RegisterListener("AddComponentstruct PhysicsBody2D", RegisterPhysicsBody);
    Dispatcher<Entity*>::RegisterListener("AddComponentstruct BoxCollider2D", RegisterCollider);
    Dispatcher<Entity*>::RegisterListener("RemoveComponentstruct PhysicsBody2D", DeregisterPhysicsBody);
    Dispatcher<Entity*>::RegisterListener("RemoveComponentstruct BoxCollider2D", DeregisterCollider);
}

void PhysicsSystem::Update() const {
    const float dampingFactor = 0.95f; // Adjust the damping factor as needed

    for (Entity* entity : _s_physics_bodies) {
        PhysicsBody2D* p_body = entity->GetComponent<PhysicsBody2D>();
        Transform* entity_transform = entity->GetComponent<Transform>();

        Vector3 acc = p_body->GetAcceleration();
        Vector3 vel = p_body->GetVelocity();
        Vector3 pos = entity_transform->GetPosition();

        // Apply damping by multiplying the velocity by the damping factor
        vel *= dampingFactor;

        vel += acc;
        acc = Vector3::zero; // Reset acceleration

        p_body->SetVelocity(vel);
        p_body->SetAcceleration(acc);

        entity_transform->SetPosition(pos + vel);
    }
    Debugger::S_LogMessage("HELLO");

}




void PhysicsSystem::RegisterPhysicsBody(Entity* entity)
{
    if (entity)
    {
        _s_physics_bodies.insert(entity);
    }
}

void PhysicsSystem::RegisterCollider(Entity* entity)
{
    if (entity)
    {
        _s_colliders.insert(entity);
    }
}

void PhysicsSystem::DeregisterCollider(Entity* entity)
{
    if (entity)
    {
        auto it = _s_colliders.find(entity);
        if (it != _s_colliders.end())
        {
            _s_colliders.erase(it);
        }
    }
}

void PhysicsSystem::DeregisterPhysicsBody(Entity* entity)
{
    if (entity)
    {
        auto it = _s_physics_bodies.find(entity);
        if (it != _s_physics_bodies.end())
        {
            _s_physics_bodies.erase(it);
        }
    }
}


void PhysicsSystem::HandleCollision(BoxCollider2D& a_box, BoxCollider2D& b_box)
{
    // Calculate collision between two box colliders
    // Implement your collision resolution logic here
}

void PhysicsSystem::HandleCollision(CircleCollider2D& a_circle, BoxCollider2D& a_box)
{
    // Calculate collision between a circle collider and a box collider
    // Implement your collision resolution logic here
}

void PhysicsSystem::HandleCollision(CircleCollider2D& a_circle, CircleCollider2D& b_circle)
{
    // Calculate collision between two circle colliders
    // Implement your collision resolution logic here
}


