#pragma once
#include <unordered_map>
#include <type_traits>
#include <array>
#include <utility>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <tuple>
#include "resource_id.h"
#include "component_sprite_pool.h"
#include "component_script_pool.h"
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
    template <typename T>
    struct PoolForType
    {
        using type = ResourcePool<T>;
    };

    template <>
    struct PoolForType<ConsoleGraphX::Sprite>
    
    {
        using type = ComponentPoolSprite;

    };

    template <>
    struct PoolForType<ConsoleGraphX::Script>

    {
        using type = ComponentPoolScript;

    };

    // Template to convert a tuple of types into a tuple of pools
    template <typename Tuple>
    struct PoolsForTypes;

    template <typename... Types>
    struct PoolsForTypes<std::tuple<Types...>>
    {
        using type = std::tuple<typename PoolForType<Types>::type...>;
    };


    class ResourceManager
    {
    private:
        static inline ResourceManager* _s_activeResourceManager = nullptr;
        PoolsForTypes<BuiltInResoruceTypes::type>::type _m_resourcePoolsT;

    private:
        template <std::size_t N = 0, typename... Ts>
        BaseResourcePool& _GetResourcePoolByIndex(std::tuple<Ts...>& tpl, std::size_t index)
        {
            if constexpr (N < sizeof...(Ts))
            {
                if (index == N)
                {
                    return std::get<N>(tpl); 
                }
                else
                {
                    return _GetResourcePoolByIndex<N + 1>(tpl, index);  // Recursively check the next index
                }
            }
            throw std::out_of_range("Index out of bounds");
        }

    public:
        static void SetActiveManager(ResourceManager* manager);
        static ResourceManager& GetActiveResourceManager();

    public:
        BaseResourcePool& GetResourcePoolFromId(ResourceID id);

        template <typename ResourceType>
        auto& GetResourcePool() 
        {
            return std::get<typename PoolForType<ResourceType>::type>(_m_resourcePoolsT);
        }

        template <typename T, typename... Args>
        std::pair<ResourceID, ResourceIndex> CreateResource(Args&&... args) 
        {
            using ResourceStorageType = typename std::conditional<IsScript<T>, ConsoleGraphX::Script, T>::type;

            ResourceID resourceId = GetResourceID<T>();
            ResourceIndex index;

            if constexpr (IsScript<T>)
            {
                ConsoleGraphX_Internal::ComponentPoolScript& pool = GetResourcePool<ConsoleGraphX::Script>();
                index = pool.CreateScript<T>(std::forward<Args>(args)...);
            }
            else
            {
                ResourcePool<ResourceStorageType>& pool = GetResourcePool<ResourceStorageType>();
                index = pool.CreateResource(std::forward<Args>(args)...);
            }

            return { resourceId, index };
        }

        std::pair<ResourceID, ResourceIndex> CreateTextureResource(const std::string& filename)
        {
            // add a way to look up textures by name to avoid reloading them
            Texture* texture = TextureLoader::LoadTexture(filename);

            if (!texture) 
            {
                throw std::runtime_error("Failed to load texture");
            }

            ResourcePool<Texture>& tPool = GetResourcePool<Texture>();

            return { GenResourceID::Get<Texture>(), tPool.PlaceResourceInPool(std::move(*texture)) };
        }

        std::pair<ResourceID, ResourceIndex> CreateTextureResource(uint32_t width, uint32_t height, int color)
        {
            Texture* texture = new Texture(width, height, color);

            ResourcePool<Texture>& tPool = GetResourcePool<Texture>();

            return { GenResourceID::Get<Texture>(), tPool.PlaceResourceInPool(std::move(*texture)) };
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
            ResourcePool<T>& pool = GetResourcePool<T>();
            return pool.GetResourceFromPool(compIndex);
        }

        template <typename T>
        void RemoveResource(ResourceIndex compIndex) 
        {
            using ResourceStorageType = typename std::conditional<IsScript<T>, ConsoleGraphX::Script, T>::type;

            ResourcePool<ResourceStorageType>& pool = GetResourcePool<T>();
            pool.RemoveResourceFromPool(compIndex);
        }

        void DestroyEntityResources(const std::unordered_map<ResourceID, ResourceIndex>& componentIdToIndexMap);
    };

};

