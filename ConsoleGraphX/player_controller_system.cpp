#include <vector>
#include "player_controller_system.h"
#include "player_controller.h"
#include "input_system.h"
#include "transform.h"
#include "vector3.h"
#include "component_manager.h"
#include "base_component_pool_impl.h"

namespace ConsoleGraphX
{

    void PlayerControllerSystem::Initialize()
    {

    }

    void PlayerControllerSystem::Update(float deltaTime)
    {
        ConsoleGraphX_Internal::BaseComponentPoolImpl<PlayerController>* controllerPool = ConsoleGraphX_Internal::ComponentManager::Instance().GetComponentPool<PlayerController>();
        std::vector<PlayerController>* controllers = controllerPool->GetPoolItems();

        ConsoleGraphX_Internal::BaseComponentPoolImpl<Transform>* tPool = ConsoleGraphX_Internal::ComponentManager::Instance().GetComponentPool<Transform>();


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