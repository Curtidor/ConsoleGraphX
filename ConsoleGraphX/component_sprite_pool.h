#pragma once
#include "base_resource_pool.h"
#include "resource_pool.h"
#include "sprite.h"

namespace ConsoleGraphX_Internal
{
    class ComponentPoolSprite : public ResourcePool<ConsoleGraphX::Sprite>
    {
    public:
        [[nodiscard]] ResourceIndex CloneComponentWithTransform(ResourceIndex index, size_t transformID)
        {
            ConsoleGraphX::Sprite* sprite = GetResourceFromPool(index);

            ResourceIndex compIndex = this->CreateResource(sprite->m_textureIndex, transformID);

            GetResourceFromPool(index)->Clone(GetResourceFromPool(compIndex));

            return compIndex;
        }
    };
};