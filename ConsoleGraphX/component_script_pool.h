#pragma once
#include <type_traits>
#include "base_resource_pool.h"
#include "resource_pool.h"
#include "script.h"
#include "resource_pool.h"


namespace ConsoleGraphX_Internal
{

    class ComponentPoolScript : public ResourcePool<ConsoleGraphX::Script*>
    {
    public:
        ComponentPoolScript(size_t maxFreeIndexThreshold): ResourcePool(maxFreeIndexThreshold)
        {}

        [[nodiscard]] ResourceIndex CloneComponentWithEntity(ResourceIndex index, ConsoleGraphX::Entity* owner)
        {
            ConsoleGraphX::Script* clonedComponent = nullptr;
            ConsoleGraphX::Script* script = GetResourceFromPool(index); 

            script->Clone(clonedComponent, owner);

            ResourceIndex newIndex = _GetOpenPoolIndex();
            _InsertIntoPool(newIndex, std::move(clonedComponent)); 

            return newIndex;
        }

        template <typename T, typename... Args>
        [[nodiscard]] ResourceIndex CreateScript(Args&&... args)
        {
            static_assert(std::is_base_of_v<ConsoleGraphX::Script, T>, "T must be derived from Script");

            ResourceIndex index = _GetOpenPoolIndex();

            this->_InsertIntoPool(index, new T(std::forward<Args>(args)...));

            return index;
        }
    };
};
