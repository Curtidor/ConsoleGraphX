#pragma once
#include <unordered_set>
#include <immintrin.h> // Include SIMD intrinsics 
#include "system.h"
#include "entity.h"
#include "box_collider2d.h"
#include "circle_collider2d.h"
#include "dispatcher.h"
#include "physics_body2d.h"
#include "debugger.h"

class PhysicsSystem : public System
{
private:
	static std::unordered_set<Entity*> _s_physics_bodies;
	static std::unordered_set<Entity*> _s_colliders;

public:
	void Initalize() const;
	void Update() const;


	static void RegisterPhysicsBody(Entity* entity);
	static void RegisterCollider(Entity* entity);
	static void DeregisterCollider(Entity* entity);
	static void DeregisterPhysicsBody(Entity* entity);

	static void HandleCollision(BoxCollider2D& a_box, BoxCollider2D& b_box);
	static void HandleCollision(CircleCollider2D& a_circle, BoxCollider2D& a_box);
	static void HandleCollision(CircleCollider2D& a_circle, CircleCollider2D& b_circle);
};