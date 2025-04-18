#include "PCH_CGX.h"
#include <limits>
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
            // TODO: Remove the need for this conditional check.
            // Currently, when an entity is destroyed, its components are reset to default values.
            // This means recycled slots in the pool may temporarily hold invalid data
            // until they're reassigned to a new entity.
            //
            // To avoid applying logic to invalid controllers, we check for a sentinel value.
            // In this case, if `m_transformID` is max(), we assume it's no longer valid.
            //
            // Potential Improvements:
            // 1. Use a `bool m_isActive` flag in each component to explicitly track usage.
            // 2. Track active indexes in the pool separately.
            // 3. Return only valid components from `GetPoolItems()`.
            if (controller.m_transformID == (std::numeric_limits<ConsoleGraphX::TransformID>::max)())
            {
                continue;
            }

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