#pragma once
#include <unordered_map>
#include <type_traits>
#include <array>
#include <utility>
#include "component.h"
#include <string>
#include <cstdint>
#include <stdexcept>
#include "resource_id.h"
#include "component_pool.h"
#include "resource_pool.h"
#include "base_resource_pool.h"
#include "meta_utils.h"
#include "script.h"
#include "sprite.h"
#include "texture.h"
#include "texture_loader.h"

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
    class ResourceManager
    {
    private:
        static inline ResourceManager* _s_instance = nullptr;
        static inline std::array<BaseResourcePool*, BuiltInResoruceTypes::count> _s_resourcePools;


    private:
        ResourceManager();

        template <typename PoolType>
        void _AllocatePoolForResourceType()
        {
            _s_resourcePools[GenResourceID::Get<PoolType>()] = new ResourcePool<PoolType>();
        }

        template<>
        void _AllocatePoolForResourceType<ConsoleGraphX::Sprite>()
        {
            _s_resourcePools[GenResourceID::Get<ConsoleGraphX::Sprite>()] = new ComponentPoolSprite();
        }

        template<>
        void _AllocatePoolForResourceType<ConsoleGraphX::Script>()
        {
            _s_resourcePools[GenResourceID::Get<ConsoleGraphX::Script>()] = new ComponentPoolScript(100);
        }

        template <typename TupleT, std::size_t... Indexes>
        void _LoopOverTuple(std::index_sequence<Indexes...>)
        {
            (_AllocatePoolForResourceType<std::tuple_element_t<Indexes, TupleT>>(), ...);
        }

        void _BuildResourcePoolArray();

        template <typename ResourceType>
        ResourcePool<ResourceType>* _GetPoolInternal(ResourceID compId) 
        {
            return static_cast<ResourcePool<ResourceType>*>(_s_resourcePools[compId]);
        }


    public:
        static void Initialize();
        static void ShutDown();
        static ResourceManager& Instance();

        BaseResourcePool* GetResourcePoolFromId(ResourceID id);

        template <typename T>
        typename std::enable_if<IsScript<T>,  ConsoleGraphX_Internal::ComponentPoolScript*>::type
            GetResourcePool() 
        {
            // Suppress warning C26498: "The function 'GenComponentID::Get' is constexpr; mark variable 'compId' constexpr if compile-time evaluation is desired."
            // This suppression is necessary because the static code analyzer incorrectly assumes that `GenComponentID::Get<ConsoleGraphX::ScriptType>()` could always be constexpr.
            // While `GenComponentID::Get` has a constexpr overload for built-in types, in this context, it selects the runtime path for non-built-in types like `Script`.
            // Since the runtime overload cannot be constexpr, the warning is a false positive, and the suppression prevents unnecessary clutter.
            #pragma warning( disable : 26498 )
            constexpr ResourceID compId = GenResourceID::Get<ConsoleGraphX::Script>();
            return static_cast<ComponentPoolScript*>(_s_resourcePools[compId]);
        }

        template <typename T>
        typename std::enable_if<!IsScript<T>, ResourcePool<T>*>::type
            constexpr GetResourcePool() 
        {
            constexpr ResourceID compId = GenResourceID::Get<T>();
            return _GetPoolInternal<T>(compId);
        }

        template <typename T, typename... Args>
        std::pair<ResourceID, ResourceIndex> CreateResource(Args&&... args) 
        {
            using ResourceStorageType = typename std::conditional<IsScript<T>, ConsoleGraphX::Script, T>::type;

            ResourceID compId = GetResourceID<T>();
            ResourceIndex index;

            if constexpr (IsScript<T>)
            {
                ConsoleGraphX_Internal::ComponentPoolScript* pool = GetResourcePool<ConsoleGraphX::Script>();
                index = pool->CreateScript<T>(std::forward<Args>(args)...);
            }
            else
            {
                ResourcePool<ResourceStorageType>* pool = GetResourcePool<ResourceStorageType>();
                index = pool->CreateResource(std::forward<Args>(args)...);
            }

            return { compId, index };
        }

        std::pair<ResourceID, ResourceIndex> CreateTextureResource(const std::string& filename)
        {
            // add a way to look up textures by name to avoid reloading them
            Texture* texture = TextureLoader::LoadTexture(filename);

            if (!texture) 
            {
                throw std::runtime_error("Failed to load texture");
            }

            ResourcePool<Texture>* tPool = GetResourcePool<Texture>();

            return { GenResourceID::Get<Texture>(), tPool->PlaceResourceInPool(std::move(*texture))};
        }

        std::pair<ResourceID, ResourceIndex> CreateTextureResource(uint32_t width, uint32_t height, int color)
        {
            Texture* texture = new Texture(width, height, color);

            ResourcePool<Texture>* tPool = GetResourcePool<Texture>();

            return { GenResourceID::Get<Texture>(), tPool->PlaceResourceInPool(std::move(*texture)) };
        }

        template <typename T>
        constexpr static typename std::enable_if<IsTypeInTuple<typename std::remove_pointer<T>::type, BuiltInResoruceTypes::type>::value, ResourceID>::type
            GetResourceID() 
        {
            return GenResourceID::Get<T>();
        }

        template <typename T>
        static typename std::enable_if<!IsTypeInTuple<typename std::remove_pointer<T>::type, BuiltInResoruceTypes::type>::value, ResourceID>::type
            GetResourceID() 
        {
            return GenResourceID::Get<T>();
        }

        template <typename T>
        T* GetResource(ResourceIndex compIndex) // this one
        {
            ResourcePool<T>* pool = GetResourcePool<T>();
            return pool->GetResourceFromPool(compIndex);
        }

        template <typename T>
        void RemoveResource(ResourceIndex compIndex) 
        {
            using ResourceStorageType = typename std::conditional<IsScript<T>, ConsoleGraphX::Script, T>::type;

            ResourcePool<ResourceStorageType>* pool = GetResourcePool<T>();
            pool->RemoveResourceFromPool(compIndex);
        }

        void DestroyEntityResources(const std::unordered_map<ResourceID, ResourceIndex>& componentIdToIndexMap);
    };

};

