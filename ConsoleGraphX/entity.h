#pragma once
#include <queue>
#include <utility>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <type_traits>
#include <stdexcept>
#include "transform.h"
#include "vector3.h"
#include "component_id.h"
#include "component_manager.h"
#include "base_component_pool.h"

namespace ConsoleGraphX_Internal
{
    struct EntityIDs
    {
    private:
        static std::queue<size_t> _s_recycledIds;
        static size_t _s_currentID;
    public:
        static size_t GetId();
        static void RecycleId(ConsoleGraphX::Entity& entity);
    };
};

namespace ConsoleGraphX
{
    /**
     * @brief A class representing an entity in the entity-component system (ECS).
     */
    class Entity
    {
    private:
        Entity* _m_parent;

        std::unordered_set<Entity*> _m_children;
       
        std::unordered_map<ConsoleGraphX_Internal::ComponentID, ConsoleGraphX_Internal::ComponentIndex> _m_componentIdToIndexMap;
        std::unordered_map<ConsoleGraphX_Internal::ComponentID, ConsoleGraphX_Internal::ComponentIndex> _m_scriptIdToIndexes;
      
    public:
        const long m_id;
        std::string m_tag;

    public:

        /**
         * @brief Default constructor for creating an entity.
         */
        Entity();

        Entity(int id);

        Entity(int id, const std::string& tag);

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
        const std::unordered_map<ConsoleGraphX_Internal::ComponentID, ConsoleGraphX_Internal::ComponentIndex>& GetComponents() const;
        const std::unordered_map<ConsoleGraphX_Internal::ComponentID, ConsoleGraphX_Internal::ComponentIndex>& GetScripts() const;

        /**
         * @brief Gets the unique ID of the entity.
         * @return The entity's ID.
         */
        long GetId() const;

        /**
         * @brief Creates a clone of the entity.
         * @return Pointer to the cloned entity.
         */
        void Clone(Entity& entity);

        /**
         * @brief Creates a clone of the entity with a specified spread for randomizing the position.
         * @param minSpread The minimum spread for randomizing the position.
         * @param maxSpread The maximum spread for randomizing the position.
         * @return Pointer to the cloned entity.
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
        ConsoleGraphX_Internal::ComponentIndex AddComponent(Args&&... args)
        {
            ConsoleGraphX_Internal::ComponentID componentId = ConsoleGraphX_Internal::ComponentManager::GetComponentID<T>();

            auto& indexMap = ConsoleGraphX_Internal::IsScript<T> ? _m_scriptIdToIndexes : _m_componentIdToIndexMap;

            if (indexMap.find(componentId) != indexMap.end())
            {
                throw std::runtime_error("Component already exists.");
            }

            std::pair<ConsoleGraphX_Internal::ComponentID, ConsoleGraphX_Internal::ComponentIndex> idIndexPair = ConsoleGraphX_Internal::ComponentManager::Instance().CreateComponent<T>(std::forward<Args>(args)...);
            indexMap.insert(idIndexPair);

            return idIndexPair.second;
        }

        /**
         * @brief Removes a component of type T from the entity.
         * @tparam T The type of component to be removed.
         */
        template <typename T>
        void RemoveComponent()
        {
            ConsoleGraphX_Internal::ComponentID componentId = ConsoleGraphX_Internal::ComponentManager::GetComponentID<T>();

            auto& indexMap = ConsoleGraphX_Internal::IsScript<T> ? _m_scriptIdToIndexes : _m_componentIdToIndexMap;

            auto it = indexMap.find(componentId);

            if (it == indexMap.end())
            {
                throw std::runtime_error("Component not found.");
            }

            ConsoleGraphX_Internal::ComponentManager::Instance().RemoveComponent<T>(it->second);
            indexMap.erase(it);
        }

        /**
        * @brief Gets a pointer to the component of the specified type.
        * @tparam T The type of the component.
        * @return A pointer to the component, or nullptr if the component does not exist.
        */
        template <typename T>
        typename std::enable_if<ConsoleGraphX_Internal::IsScript<T>, T*>::type
            GetComponent()
        {
            ConsoleGraphX_Internal::ComponentID componentId = ConsoleGraphX_Internal::ComponentManager::GetComponentID<T>();
            return _GetComponentImpl<T>(componentId, _m_scriptIdToIndexes);
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
            constexpr ConsoleGraphX_Internal::ComponentID componentId = ConsoleGraphX_Internal::ComponentManager::GetComponentID<T>();
            return _GetComponentImpl<T>(componentId, _m_componentIdToIndexMap);
        }

        Transform* GetTransform();

        void DestroyEntity() const
        {
            ConsoleGraphX_Internal::ComponentManager::Instance().DestroyEntityComponents(_m_componentIdToIndexMap);
            ConsoleGraphX_Internal::ComponentManager::Instance().DestroyEntityComponents(_m_scriptIdToIndexes);
        }

        /**
         * @brief Equality operator for comparing entities.
         * @param other The entity to compare with.
         * @return True if the entities are equal, false otherwise.
         */
        struct Hash
        {
            using is_transparent = void;

            size_t operator()(const Entity& entity) const
            {
                return std::hash<int>()(entity.m_id);
            }

            size_t operator()(int id) const
            {
                return std::hash<int>()(id);
            }
        };

        struct Equal
        {
            using is_transparent = void;

            bool operator()(const Entity& lhs, const Entity& rhs) const
            {
                return lhs.m_id == rhs.m_id && lhs.m_tag == rhs.m_tag;
            }

            bool operator()(const Entity& entity, int id) const
            {
                return entity.m_id == id;
            }

            bool operator()(int id, const Entity& entity) const
            {
                return id == entity.m_id;
            }
        };

        /**
         * @brief Inequality operator for comparing entities.
         * @param other The entity to compare with.
         * @return True if the entities are not equal, false otherwise.
         */
        bool operator!=(const Entity& other) const;

    private:
        // The reason we split the function into is script and not is to allow for the use of constexpr
        // as during profiling getting builtin componets was a hot spot
        /**
        * @brief Helper function to retrieve a component.
        * @tparam T The type of the component.
        * @param componentId The ID of the component.
        * @param indexMap The map to use for the lookup.
        * @return A pointer to the component, or nullptr if the component does not exist.
        */
        template <typename T>
        inline T* _GetComponentImpl(ConsoleGraphX_Internal::ComponentID componentId, const std::unordered_map<ConsoleGraphX_Internal::ComponentID, size_t>& indexMap)
        {
            auto it = indexMap.find(componentId);
            if (it == indexMap.end())
            {
                return nullptr;
            }

            return ConsoleGraphX_Internal::ComponentManager::Instance().GetComponent<T>(it->second);
        }
    };
};
