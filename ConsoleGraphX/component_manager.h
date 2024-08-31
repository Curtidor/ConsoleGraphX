#pragma once
#include <unordered_map>
#include <type_traits>
#include <array>
#include <utility>
#include "component.h"
#include "component_id.h"
#include "component_pool.h"
#include "base_component_pool.h"
#include "meta_utilities.h"

// OVERVIEW:
// Each type of component gets its own pool. However, components that inherit from `Script` share a pool.
// This design simplifies management of script components by avoiding multiple pools.
//
// WHY:
// 1. The script manager can efficiently access all scripts without handling multiple pools.
//
// Q&A:
// Q: How are script components differentiated if they share a pool?
// A: The script system updates all scripts collectively, so their types are handled uniformly,
//    without needing differentiation. However, each subclass of `Script` is treated like a normal
//    component with a unique ID. This allows the `GetComponent<SomeScript>()` call to work similarly
//    to non-script components. Scripts are managed in a separate map using a [ScriptID : IndexInPool]
//    structure, this design choice means we can 

namespace ConsoleGraphX_Internal
{
    class ComponentManager
    {
    private:
        std::array<BaseComponentPool*, BuiltInComponentTypes::count> _m_componentPools;

        ComponentManager();

        template <typename PoolType>
        void _AllocPoolItem()
        {
            _m_componentPools[GenComponentID::Get<PoolType>()] = new ComponentPool<PoolType>();
        }

        template <typename TupleT, std::size_t... Indexes>
        void _LoopOverTuple(std::index_sequence<Indexes...>)
        {
            (_AllocPoolItem<std::tuple_element_t<Indexes, TupleT>>(), ...);
        }

        void _BuildComponentPoolArray();

        template <typename ComponentType>
        ComponentPool<ComponentType>* _GetPoolInternal(ComponentID compId) {
            return static_cast<ComponentPool<ComponentType>*>(_m_componentPools[compId]);
        }

    public:
        static ComponentManager& Instance();

        void FreePools();

        BaseComponentPool* GetComponentPoolFromId(ComponentID id);

        template <typename T>
        typename std::enable_if<IsScript<T>, ComponentPool<ConsoleGraphX::Script>*>::type
            GetComponentPool() 
        {
            ComponentID compId = GenComponentID::Get<ConsoleGraphX::Script>();
            return _GetPoolInternal<ConsoleGraphX::Script>(compId);
        }

        template <typename T>
        typename std::enable_if<!IsScript<T>, ComponentPool<T>*>::type
            constexpr GetComponentPool() 
        {
            constexpr ComponentID compId = GenComponentID::Get<T>();
            return _GetPoolInternal<T>(compId);
        }

        template <typename T, typename... Args>
        std::pair<ComponentID, ComponentIndex> CreateComponent(Args&&... args) 
        {
            static_assert(std::is_base_of<Component, T>::value, "Type must inherit from Component");

            using ComponentStorageType = typename std::conditional<IsScript<T>, ConsoleGraphX::Script, T>::type;

            ComponentID compId = GetComponentID<T>();

            ComponentPool<ComponentStorageType>* pool = GetComponentPool<ComponentStorageType>();
            ComponentIndex index = pool->CreateComponent<T>(std::forward<Args>(args)...);

            return { compId, index };
        }

        template <typename T>
        constexpr static typename std::enable_if<IsTypeInTuple<typename StripPointer<T>::type, BuiltInComponentTypes::type>::value, ComponentID>::type
            GetComponentID() 
        {
            return GenComponentID::Get<T>();
        }

        template <typename T>
        static typename std::enable_if<!IsTypeInTuple<typename StripPointer<T>::type, BuiltInComponentTypes::type>::value, ComponentID>::type
            GetComponentID() 
        {
            return GenComponentID::Get<T>();
        }

        template <typename T>
        T* GetComponent(ComponentIndex compIndex) {
            ComponentPool<T>* pool = GetComponentPool<T>();
            return pool->GetComponentFromPool(compIndex);
        }

        template <typename T>
        void RemoveComponent(ComponentIndex compIndex) 
        {
            using ComponentStorageType = typename std::conditional<IsScript<T>, ConsoleGraphX::Script, T>::type;

            ComponentPool<ComponentStorageType>* pool = GetComponentPool<T>();
            pool->RemoveComponentFromPool(compIndex);
        }

        void DestroyEntityComponents(const std::unordered_map<ComponentID, ComponentIndex>& componentIdToIndexMap);
    };

};

