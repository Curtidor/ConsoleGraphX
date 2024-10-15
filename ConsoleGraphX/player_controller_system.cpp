#include "CGXPCH.h"
#include "player_controller_system.h"
#include "player_controller.h"
#include "input_system.h"
#include "transform.h"
#include "resource_manager.h"
#include "resource_pool.h"

namespace ConsoleGraphX
{

    void PlayerControllerSystem::Initialize()
    {

    }

    void PlayerControllerSystem::Update(float deltaTime)
    {
        ConsoleGraphX_Internal::ResourcePool<PlayerController>& controllerPool = ConsoleGraphX_Internal::ResourceManager::GetActiveResourceManager().GetResourcePool<PlayerController>();
        std::vector<PlayerController>* controllers = controllerPool.GetPoolItems();

        for (PlayerController& controller : *controllers)
        {
            Transform* transform = controller.GetTransform();

            controller.m_velocity.y -= controller.m_gravity * deltaTime;

            Vector3 movement;

            if (InputSystem::IsKeyPressed(Key::A))
            {
                movement += Vector3::left;
            }
            else if (InputSystem::IsKeyPressed(Key::D))
            {
                movement += Vector3::right;
            }

            if (InputSystem::IsKeyPressed(Key::W))
            {
                movement += Vector3::down;
            }
            else if (InputSystem::IsKeyPressed(Key::S))
            {
                movement += Vector3::up;
            }

            transform->Translate(movement * controller.m_moveSpeed * deltaTime);

        }
    }
};