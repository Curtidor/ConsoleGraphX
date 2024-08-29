#pragma once
#include <vector>
#include <queue>
#include <type_traits>
#include "base_component_pool.h"


/**
 * @brief A component pool for managing components of type T.
 * @tparam ComponentStorageType The type of components managed by this pool.
 */
namespace ConsoleGraphX_Internal
{
    template <typename ComponentStorageType>
    class ComponentPool : public BaseComponentPool
    {
    private:
        std::vector<ComponentStorageType> _m_componentPool;
        std::queue<ComponentIndex> _m_openPoolIndexes;

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

    public:
        const std::vector<ComponentStorageType>* GetPoolItems() const
        {
            return &_m_componentPool;
        }

        template <typename T, typename... Args>
        [[nodiscard]] ComponentIndex CreateComponent(Args&&... args)
        {
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

        ComponentStorageType* GetComponentFromPool(ComponentIndex compIndex)
        {
            if (compIndex < 0 || compIndex >= _m_componentPool.size())
            {
                return nullptr;
            }

            return &_m_componentPool[compIndex];
        }

        void RemoveComponentFromPool(ComponentIndex index) override
        {
            if (index < 0 || index >= _m_componentPool.size())
                return;

            _m_openPoolIndexes.push(index);
        }
    };


    template <>
    class ComponentPool<ConsoleGraphX::Script> : public BaseComponentPool
    {
    private:
        std::vector<ConsoleGraphX::Script*> _m_componentPool;
        std::queue<ComponentIndex> _m_openPoolIndexes;

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


    public:
        const std::vector<ConsoleGraphX::Script*>* GetPoolItems() const
        {
            return &_m_componentPool;
        }

        template <typename T, typename... Args>
        [[nodiscard]] ComponentIndex CreateComponent(Args&&... args)
        {
            static_assert(std::is_base_of_v<ConsoleGraphX::Script, T>, "T must be derived from Script");

            ComponentIndex index = _GetOpenPoolIndex();
            if (index == _m_componentPool.size())
            {
                _m_componentPool.push_back(new T(std::forward<Args>(args)...));
            }
            else
            {
                _m_componentPool[index] = new T(std::forward<Args>(args)...);
            }

            return index;
        }

        ConsoleGraphX::Script* GetComponentFromPool(ComponentIndex compIndex)
        {
            if (compIndex < 0 || compIndex >= _m_componentPool.size())
            {
                return nullptr;
            }

            return _m_componentPool[compIndex];
        }

        void RemoveComponentFromPool(ComponentIndex index) override
        {
            if (index < 0 || index >= _m_componentPool.size())
                return;

            delete _m_componentPool[index];
            _m_openPoolIndexes.push(index);
        }
    };
};
