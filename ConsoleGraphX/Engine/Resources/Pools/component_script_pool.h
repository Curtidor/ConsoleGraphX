#pragma once
#include <type_traits>
#include "Engine\Resources\Pools\base_resource_pool.h"
#include "Engine\Resources\Pools\resource_pool.h"
#include "Engine\Components\script.h"
#include "Engine\Resources\Pools\resource_pool.h"


namespace ConsoleGraphX_Internal
{

    class ComponentPoolScript : public ResourcePool<ConsoleGraphX::Script*>
    {
    public:
        ComponentPoolScript()
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
