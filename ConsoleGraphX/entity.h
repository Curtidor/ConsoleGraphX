#pragma once
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <type_traits>
#include <stdexcept>
#include "transform.h"
#include "vector3.h"
#include "resource_id.h"
#include "resource_manager.h"
#include "base_resource_pool.h"
#include "position_component.h"
#include "script.h"
#include "component.h"

namespace ConsoleGraphX_Internal
{
    struct EntityIDs
    {
    private:
        static inline std::queue<size_t> _s_recycledIds;
        static inline size_t _s_currentID = 0;

    public:
        static size_t GetId();
        static void RecycleId(ConsoleGraphX::Entity& entity);
    };
};

namespace ConsoleGraphX
{
  // ARCHITECTURE NOTE FOR FUTURE ME:
  // CHALLENGE: Accessing the correct ResourceManager in _GetComponentImpl
  // NOTE ON THE TOPIC:
  //      When calling GetComponent on an entity, we cannot guarantee that the entity in question 
  //      is part of the currently active scene. Thus, simply using 
  //      ResourceManager::GetActiveManager().GetResource<T>(it->second); might return incorrect resources, 
  //      since the active manager corresponds to the active scene.
  //
  // PROPOSED SOLUTIONS:
  //      1. Dependency Injection (DI): Since scenes are responsible for creating entities, 
  //         we can easily inject a reference to the scene's ResourceManager into the entity. This would allow for:
  //         _m_resourceManager.GetResource<T>(it->second);
  //
  //      2. Scene ID Lookup: Alternatively, we could pass a scene ID to the entity. This decouples the entity 
  //         from the ResourceManager but introduces a more costly lookup:
  //         SceneSystem::GetScene(_m_sceneId)->GetResourceManager().GetResource<T>(it->second);
  //
  // CONCLUSION:
  //      For now, DI (Dependency Injection) is the preferred approach.



    /**
     * @brief A class representing an entity in the entity-component system (ECS).
     */
    class Entity
    {
    private:
        Entity* _m_parent;
        ConsoleGraphX_Internal::ResourceManager& _m_resourceManager; // Injected via DI

        std::unordered_set<Entity*> _m_children;
       
        std::unordered_map<ConsoleGraphX_Internal::ResourceID, ConsoleGraphX_Internal::ResourceIndex> _m_componentIdToIndexMap;
        std::unordered_map<ConsoleGraphX_Internal::ResourceID, ConsoleGraphX_Internal::ResourceIndex> _m_scriptIdToIndexes;
    private:
        void _CheckComponentExists(ConsoleGraphX_Internal::ResourceID componentId, const std::unordered_map<ConsoleGraphX_Internal::ResourceID, ConsoleGraphX_Internal::ResourceIndex>& indexMap);

        template <typename T>
        std::unordered_map<ConsoleGraphX_Internal::ResourceID, ConsoleGraphX_Internal::ResourceIndex>& GetIndexMap() {
            if constexpr (ConsoleGraphX_Internal::IsScript<T>) 
            {
                return _m_scriptIdToIndexes;
            }
            else 
            {
                return _m_componentIdToIndexMap;
            }
        }


        template <typename T, typename... Args>
        ConsoleGraphX_Internal::ResourceIndex CreateComponentInPool(Args&&... args) {
            if constexpr (std::is_base_of<ConsoleGraphX_Internal::PositionComponentBase, T>::value) 
            {
                return _m_resourceManager.CreateResource<T>(std::forward<Args>(args)..., _m_componentIdToIndexMap[ConsoleGraphX_Internal::GenResourceID::Get<Transform>()]).second;
            }
            else if constexpr (ConsoleGraphX_Internal::IsScript<T>)
            {
                return _m_resourceManager.CreateResource<T>(std::forward<Args>(args)..., this).second;
            }
            else 
            {
                return _m_resourceManager.CreateResource<T>(std::forward<Args>(args)...).second;
            }
        }

    public:
        const size_t m_id;
        std::string m_tag;

    public:

        /**
         * @brief Default constructor for creating an entity.
         */
        Entity(int id);
        Entity(ConsoleGraphX_Internal::ResourceManager& resourceManager);

        Entity(ConsoleGraphX_Internal::ResourceManager& resourceManager, int id);

        Entity(ConsoleGraphX_Internal::ResourceManager& resourceManager, int id, const std::string& tag);

        ~Entity();

        /**
         * @brief Sets the parent of the entity.
         * @param newParent The new parent entity.
         */
        void SetParent(Entity* newParent);

        /**
         * @brief Adds a child entity to the current entity.
         * @param child The child entity to be added.
         */
        void AddChild(Entity* child);

        /**
         * @brief Removes a child entity from the current entity.
         * @param child The child entity to be removed.
         */
        void RemoveChild(Entity* child);

        /**
         * @brief Destroys the entity, removing it from the ECS.
         */
        void KillEntity();

        /**
         * @brief Gets a reference to the components associated with this entity.
         * @return A reference to the components map.
         */
        const std::unordered_map<ConsoleGraphX_Internal::ResourceID, ConsoleGraphX_Internal::ResourceIndex>& GetComponents() const;
        const std::unordered_map<ConsoleGraphX_Internal::ResourceID, ConsoleGraphX_Internal::ResourceIndex>& GetScripts() const;

        /**
         * @brief Gets the unique ID of the entity.
         * @return The entity's ID.
         */
        size_t GetId() const;

        /**
         * @brief Creates a clone of the entity.
         */
        void Clone(Entity& entity);

        /**
         * @brief Creates a clone of the entity with a specified spread for randomizing the position.
         * @param minSpread The minimum spread for randomizing the position.
         * @param maxSpread The maximum spread for randomizing the position.
         */
        void Clone(Entity& entity, const Vector3& minSpread, const Vector3& maxSpread);

        /**
         * @brief Adds a new component of type T to the entity.
         * @tparam T The type of component to be added.
         * @tparam Args Variadic template for component constructor arguments.
         * @param args The arguments to be forwarded to the component constructor.
         * @return Pointer to the added component.
         */
        template <typename T, typename... Args>
        ConsoleGraphX_Internal::ResourceIndex AddComponent(Args&&... args) 
        {
            static_assert(std::is_base_of_v<ConsoleGraphX_Internal::Component, T>, "T must be derived from the Component Type");

            ConsoleGraphX_Internal::ResourceID componentId = ConsoleGraphX_Internal::ResourceManager::GetResourceID<T>();

            auto& indexMap = GetIndexMap<T>();
            _CheckComponentExists(componentId, indexMap);

            ConsoleGraphX_Internal::ResourceIndex compIndex = CreateComponentInPool<T>(std::forward<Args>(args)...);
            indexMap.emplace(componentId, compIndex);

            return compIndex;
        }

        /**
         * @brief Removes a component of type T from the entity.
         * @tparam T The type of component to be removed.
         */
        template <typename T>
        void RemoveComponent()
        {
            static_assert(std::is_base_of_v<ConsoleGraphX_Internal::Component, T>, "T must be derived from the Component Type");
            static_assert(std::is_same_v<T, Transform>, "Cannot Remove Transforms!");

            ConsoleGraphX_Internal::ResourceID componentId = ConsoleGraphX_Internal::ResourceManager::GetResourceID<T>();

            // possible constexpr
            auto& indexMap = ConsoleGraphX_Internal::IsScript<T> ? _m_scriptIdToIndexes : _m_componentIdToIndexMap;

            auto it = indexMap.find(componentId);

            if (it == indexMap.end())
            {
                throw std::runtime_error("Component not found.");
            }

            _m_resourceManager.RemoveResource<T>(it->second);
            indexMap.erase(it);
        }

        /**
        * @brief Gets a pointer to the component of the specified type.
        * @tparam T The type of the component.
        * @return A pointer to the component, or nullptr if the component does not exist.
        */
        template <typename T>
        typename std::enable_if<ConsoleGraphX_Internal::IsScript<T>, Script*>::type
            GetComponent()
        {
            static_assert(std::is_base_of_v<ConsoleGraphX_Internal::Component, T>, "T must be derived from the Component Type");

            ConsoleGraphX_Internal::ResourceID componentId = ConsoleGraphX_Internal::ResourceManager::GetResourceID<T>();
            return _GetComponentImpl<Script>(componentId, _m_scriptIdToIndexes);
        }

        /**
         * @brief Gets a pointer to the component of the specified type.
         * @tparam T The type of the component.
         * @return A pointer to the component, or nullptr if the component does not exist.
         */
        template <typename T>
        typename std::enable_if<!ConsoleGraphX_Internal::IsScript<T>, T*>::type
            GetComponent()
        {
            static_assert(std::is_base_of_v<ConsoleGraphX_Internal::Component, T>, "T must be derived from the Component Type");

            constexpr ConsoleGraphX_Internal::ResourceID componentId = ConsoleGraphX_Internal::ResourceManager::GetResourceID<T>();
            return _GetComponentImpl<T>(componentId, _m_componentIdToIndexMap);
        }

        Transform* GetTransform();

        void DestroyEntity() const;

        /**
         * @brief Equality operator for comparing entities.
         * @param other The entity to compare with.
         * @return True if the entities are equal, false otherwise.
         */
        struct Hash
        {
            using is_transparent = void;

            size_t operator()(const Entity& entity) const;
            size_t operator()(int id) const;
        };

        struct Equal
        {
            using is_transparent = void;

            bool operator()(const Entity& lhs, const Entity& rhs) const;
            bool operator()(const Entity& entity, int id) const;
            bool operator()(int id, const Entity& entity) const;
        };
        /**
         * @brief Inequality operator for comparing entities.
         * @param other The entity to compare with.
         * @return True if the entities are not equal, false otherwise.
         */
        bool operator!=(const Entity& other) const;

    private:
        // The reason we split the function into is script and not is to allow for the use of constexpr
        // as during profiling getting builtin components was a hot spot
        /**
        * @brief Helper function to retrieve a component.
        * @tparam T The type of the component.
        * @param componentId The ID of the component.
        * @param indexMap The map to use for the lookup.
        * @return A pointer to the component, or nullptr if the component does not exist.
        */
        template <typename T>
        inline T* _GetComponentImpl(ConsoleGraphX_Internal::ResourceID componentId, const std::unordered_map<ConsoleGraphX_Internal::ResourceID, size_t>& indexMap)
        {
            auto it = indexMap.find(componentId);
            if (it == indexMap.end())
            {
                return nullptr;
            }

            return _m_resourceManager.GetResource<T>(it->second);
        }
    };
};
