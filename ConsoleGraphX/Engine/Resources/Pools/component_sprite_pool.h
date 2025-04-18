#pragma once
#include "Engine\Resources\Pools\base_resource_pool.h"
#include "Engine\Resources\Pools\resource_pool.h"
#include "Engine\Resources\resource_manager.h"
#include "Engine\Components\PositionComponents\sprite.h"

namespace ConsoleGraphX_Internal
{
    class ComponentPoolSprite : public ResourcePool<ConsoleGraphX::Sprite>
    {
    public:
        [[nodiscard]] ResourceIndex CloneComponentWithTransform(ResourceIndex index, ResourceManager* rManager, size_t transformID)
        {
            ConsoleGraphX::Sprite* sprite = GetResourceFromPool(index);

            ResourceIndex compIndex = this->CreateResource(sprite->m_textureIndex, rManager, transformID);

            GetResourceFromPool(index)->Clone(GetResourceFromPool(compIndex));

            return compIndex;
        }
    };
};