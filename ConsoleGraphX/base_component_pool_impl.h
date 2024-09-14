#pragma once
#include <vector>
#include <type_traits>
#include "base_component_pool.h"
#include "meta_utilities.h"
#include "component_id.h"

namespace ConsoleGraphX_Internal
{
    template <typename ComponentStorageType>
    class BaseComponentPoolImpl : public BaseComponentPool
    {
    protected:
        std::vector<ComponentStorageType> _m_componentPool;

        [[nodiscard]] ComponentIndex _GetOpenPoolIndex()
        {
            if (!_m_openPoolIndexes.empty())
            {
                ComponentIndex index = _m_openPoolIndexes.front();
                _m_openPoolIndexes.pop();
                return index;
            }

            return _m_componentPool.size();
        }

        void _InsertComponentIntoPool(ComponentIndex index, const ComponentStorageType& component)
        {
            if (index == _m_componentPool.size())
            {
                _m_componentPool.push_back(component);
            }
            else
            {
                _m_componentPool[index] = component;
            }
        }

    public:
        BaseComponentPoolImpl()
        {
            _m_componentPool.reserve(100);
        }

        template <typename T, typename... Args>
        [[nodiscard]] ComponentIndex CreateComponent(Args&&... args)
        {
            static_assert(std::is_constructible_v<ComponentStorageType, Args...>, "ComponentStorageType is not constructible with the provided arguments!");

            ComponentIndex index = _GetOpenPoolIndex();
            if (index == _m_componentPool.size())
            {
                _m_componentPool.emplace_back(std::forward<Args>(args)...);
            }
            else
            {
                _m_componentPool[index] = ComponentStorageType(std::forward<Args>(args)...);
            }

            return index;
        }

        [[nodiscard]] ComponentIndex PlaceComponentInPool(ComponentStorageType comp)
        {
            ComponentIndex index = _GetOpenPoolIndex();

            if (index == _m_componentPool.size())
            {
                _m_componentPool.push_back(std::move(comp));
            }
            else
            {
                _m_componentPool[index] = std::move(comp);
            }

            return index;
        }

        std::vector<ComponentStorageType>* GetPoolItems()
        {
            return &_m_componentPool;
        }


        [[nodiscard]] ComponentIndex CloneComponent(ComponentIndex index) override
        {
            ComponentIndex compIndex = CreateComponent<ComponentStorageType>();

            auto originalComponent = GetComponentFromPool(index);
            auto clonedComponent = GetComponentFromPool(compIndex);
            
            originalComponent->Clone(clonedComponent);

            return compIndex;
        }

        typename StripPointer<ComponentStorageType>::type* GetComponentFromPool(ComponentIndex compIndex)
        {
        #ifdef _DEBUG
            if (compIndex < 0 || compIndex >= _m_componentPool.size())
            {
                return nullptr;
            }
        #endif // _DEBUG

            if constexpr (IsScript<ComponentStorageType>)
            {
                return _m_componentPool[compIndex];
            }
            else
            {
                return &_m_componentPool[compIndex];
            }
        }

        void RemoveComponentFromPool(ComponentIndex index) override
        {
            if (index < 0 || index >= _m_componentPool.size())
                return;

            _m_openPoolIndexes.push(index);
        }
    };
};

