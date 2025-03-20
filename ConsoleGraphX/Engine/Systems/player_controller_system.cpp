#include "PCH_CGX.h"
#include "Engine\Systems\player_controller_system.h"
#include "Engine\Components\PositionComponents\player_controller.h"
#include "Engine\Systems\input_system.h"
#include "Engine\Components\transform.h"
#include "Engine\Resources\resource_manager.h"
#include "Engine\Resources\Pools\resource_pool.h"
#include "Engine\Systems\scene_system.h"

namespace ConsoleGraphX
{

    void PlayerControllerSystem::Initialize()
    {}

    void PlayerControllerSystem::Update(float deltaTime, SceneSystem& sceneSystem)
    {
        ConsoleGraphX_Internal::ResourcePool<PlayerController>& controllerPool = sceneSystem.GetActiveResourceManager().GetResourcePool<PlayerController>();
        std::vector<PlayerController>* controllers = controllerPool.GetPoolItems();

        for (PlayerController& controller : *controllers)
        {
            Transform* transform = controller.GetTransform();

            controller.m_velocity.y -= controller.m_gravity * deltaTime;

            Vector3 movement;

            InputSystem& inputSystem = InputSystem::Instance();
            if (inputSystem.IsKeyPressed(Key::A))
            {
                movement += Vector3::left;
            }
            else if (inputSystem.IsKeyPressed(Key::D))
            {
                movement += Vector3::right;
            }

            if (inputSystem.IsKeyPressed(Key::W))
            {
                movement += Vector3::down;
            }
            else if (inputSystem.IsKeyPressed(Key::S))
            {
                movement += Vector3::up;
            }

            transform->Translate(movement * controller.m_moveSpeed * deltaTime);

        }
    }
};