#pragma once
#include <vector>
#include <type_traits>
#include "base_component_pool.h"
#include "base_component_pool_impl.h"
#include "script.h"
#include "sprite.h"


/**
 * @brief A component pool for managing components of type T.
 * @tparam ComponentStorageType The type of components managed by this pool.
 */
namespace ConsoleGraphX_Internal
{
    class ComponentPoolSprite : public BaseComponentPoolImpl<ConsoleGraphX::Sprite>
    {
    public:
        [[nodiscard]] ComponentIndex CloneComponentWithTransform(ComponentIndex index, size_t transformID)
        {
            ComponentIndex compIndex = CreateComponent<ConsoleGraphX::Sprite>(transformID); // MAIN ARG ERROR SOURCE

            GetComponentFromPool(index)->Clone(GetComponentFromPool(compIndex));

            return compIndex;
        }
    };

    class ComponentPoolScript : public BaseComponentPoolImpl<ConsoleGraphX::Script*>
    {
    public:
        template <typename T, typename... Args>
        [[nodiscard]] ComponentIndex CreateComponent(Args&&... args)
        {
            static_assert(std::is_base_of_v<ConsoleGraphX::Script, T>, "T must be derived from Script");

            ComponentIndex index = _GetOpenPoolIndex();

            _InsertComponentIntoPool(index, new T(std::forward<Args>(args)...));

            return index;
        }

        [[nodiscard]] ComponentIndex CloneComponentWithEntity(ComponentIndex index, ConsoleGraphX::Entity* owner)
        {
            ConsoleGraphX::Script* clonedComponent = nullptr;
            ConsoleGraphX::Script* script = GetComponentFromPool(index); // Clone should allocate the appropriate derived type.

            script->Clone(clonedComponent, owner);

            ComponentIndex newIndex = _GetOpenPoolIndex();
            _InsertComponentIntoPool(newIndex, clonedComponent);

            return newIndex;
        }

        void RemoveComponentFromPool(ComponentIndex index) override
        {
            if (index < 0 || index >= _m_componentPool.size())
                return;

            delete _m_componentPool[index];
            _m_componentPool[index] = nullptr;
            _m_openPoolIndexes.push(index);
        }
    };
};
