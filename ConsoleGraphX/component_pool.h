#pragma once
#include <vector>
#include <queue>
#include <type_traits>
#include "base_component_pool.h"

namespace ConsoleGraphX
{
    struct Script;
};

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

        std::vector<ComponentStorageType>* GetPoolItems()
        {
            return &_m_componentPool;
        }

        [[nodiscard]] ComponentIndex CloneComponent(ComponentIndex index) override
        {
            ComponentIndex compIndex = CreateComponent<ComponentStorageType>();

            GetComponentFromPool(index)->Clone(GetComponentFromPool(compIndex));

            return compIndex;
        }

        ComponentStorageType* GetComponentFromPool(ComponentIndex compIndex)
        {
        #ifdef _DEBUG
            if (compIndex < 0 || compIndex >= _m_componentPool.size())
            {
                return nullptr;
            }
        #endif // _DEBUG

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

        void _InsertComponentIntoPool(ComponentIndex index, ConsoleGraphX::Script* script)
        {
            if (index == _m_componentPool.size())
            {
                _m_componentPool.push_back(script);
            }
            else
            {
                _m_componentPool[index] = script;
            }
        }

    public:
        template <typename T, typename... Args>
        [[nodiscard]] ComponentIndex CreateComponent(Args&&... args)
        {
            static_assert(std::is_base_of_v<ConsoleGraphX::Script, T>, "T must be derived from Script");

            ComponentIndex index = _GetOpenPoolIndex();

            _InsertComponentIntoPool(index, new T(std::forward<Args>(args)...));

            return index;
        }

        [[nodiscard]] ComponentIndex CloneComponent(ComponentIndex index) override
        {
            ConsoleGraphX::Script* clonedComponent = nullptr;
            GetComponentFromPool(index)->Clone(clonedComponent); // Clone should allocate the appropriate derived type.

            ComponentIndex newIndex = _GetOpenPoolIndex();
            _InsertComponentIntoPool(newIndex, static_cast<ConsoleGraphX::Script*>(clonedComponent));

            return newIndex;
        }


        ConsoleGraphX::Script* GetComponentFromPool(ComponentIndex compIndex)
        {
        #ifdef _DEBUG
            if (compIndex < 0 || compIndex >= _m_componentPool.size())
            {
                return nullptr;
            }
        #endif

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
