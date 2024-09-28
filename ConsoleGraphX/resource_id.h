#pragma once
#include <utility>
#include <type_traits>
#include "meta_utils.h"
#include "component.h"
#include "transform.h"
#include "player_controller.h"
#include "camera.h"
#include "sprite.h"
#include "script.h"
#include "texture.h"

namespace  ConsoleGraphX_Internal
{
    using ResourceID = size_t;

    // NOTE TO FUTURE ME
    // This whole compile-time ID thing was more of a "let's dive into metaprogramming" experiment
    // rather than a serious attempt at optimizing the code. 
    // Feel free to simplify things if it become a problem in terms of maintenance 
    // (which it shouldn't, at the time of writing this as only the registry needs updating) 
    // as the runtime system is pretty efficient and doesn't require forward dec.
    // HOWEVER, remember, runtime id's do introduce some minimal overhead, which maybe be 
    // important as some systems work with builtin components in tight loops.


    template <typename ResourceType>
    static constexpr bool IsScript = std::is_base_of_v<ConsoleGraphX::Script, typename std::remove_pointer<ResourceType>::type>;


    /**
    * @brief A registry that holds a tuple of component types.
    *
    * This template struct stores a collection of component types and provides
    * a mechanism to manage and index these types.
    *
    * @tparam Ts The component types to be managed.
    */
    template <typename... Ts>
    struct ResourceTypeRegistry
    {
        using type = std::tuple<Ts...>;

        static constexpr std::size_t count = sizeof...(Ts); // number of component types
    };

    // IF NEW COMPONENTS ARE ADDED/REMOVED UPDATE THIS REGISTRY AND FOWARD DEC's NO OTHER CODE NEEDS TO BE TOUCHED
    using BuiltInResoruceTypes = ResourceTypeRegistry<ConsoleGraphX::Script, ConsoleGraphX::Transform, ConsoleGraphX::PlayerController, ConsoleGraphX::Camera, ConsoleGraphX::Sprite, Texture>;

    /**
        * @brief Provides unique IDs for components.
        *
        * This class generates and retrieves unique IDs for built-in and custom component types.
        * Built-in component types receive compile-time IDs, while custom types receive runtime IDs.
        */
    struct GenResourceID
    {
        /**
            * @brief Gets the unique ID for built-in component types.
            *
            * This method retrieves the ID based on the index in the tuple of built-in component types.
            *
            * @tparam T The component type for which to get the ID.
            * @return The unique ID for the component type.
            */
        template <typename T>
        constexpr static typename std::enable_if<IsTypeInTuple<typename std::remove_pointer<T>::type, BuiltInResoruceTypes::type>::value, ResourceID>::type
            Get()
        {
            return IndexInTuple<typename std::remove_pointer<T>::type, BuiltInResoruceTypes::type>::value;
        }

        /**
            * @brief Gets the unique ID for custom component types.
            *
            * This method generates a unique ID at runtime for component types that are not built-in.
            *
            * @tparam T The component type for which to get the ID.
            * @return The unique ID for the component type.
            */
        template <typename T>
        static typename std::enable_if<!IsTypeInTuple<typename std::remove_pointer<T>::type, BuiltInResoruceTypes::type>::value, ResourceID>::type
            Get()
        {
            static_assert(std::is_base_of_v<Component, typename std::remove_pointer<T>::type>, "Type must inherit from Component!");

            static ResourceID id = nextID++;
            return id;
        }

    private:
        static inline size_t nextID = BuiltInResoruceTypes::count; // tracks the next available ID
    };

    static inline bool IsScriptFromID(ResourceID id)
    {
        // id == 0 or id >= all built in component types
        return id == GenResourceID::Get<ConsoleGraphX::Script>() || id >= BuiltInResoruceTypes::count;
    }
};
