#pragma once
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <string>
#include <type_traits>
#include "component.h"
#include "vector3.h"

namespace ConsoleGraphX
{
    /**
     * @brief A class representing an entity in the entity-component system (ECS).
     */
    class Entity
    {
    private:
        static long _s_totalEntities;

        Entity* _m_parent;
        std::unordered_set<Entity*> _m_children;
        std::unordered_map<std::type_index, std::unique_ptr<ConsoleGraphX_Internal::Component>> _m_components;
        std::unordered_map<std::type_index, std::unique_ptr<ConsoleGraphX_Internal::Component>> _m_scripts;

        std::unordered_map<std::type_index, std::unique_ptr<ConsoleGraphX_Internal::Component>>::iterator _RemoveScript(std::type_index index, ConsoleGraphX_Internal::Component* script);
        std::unordered_map<std::type_index, std::unique_ptr<ConsoleGraphX_Internal::Component>>::iterator _RemoveComponent(std::type_index type, ConsoleGraphX_Internal::Component* comp);


        void _CloneComponents(Entity* spawnedEntity) const;
        void _AddComponent(std::type_index inded, std::unique_ptr<ConsoleGraphX_Internal::Component> comp);
        
      
    public:
        const long m_id;
        const std::string m_name;

        /**
         * @brief Default constructor for creating an entity.
         */
        Entity();

        /**
         * @brief Constructor for creating an entity with a specified name.
         * @param entityName The name of the entity.
         */
        Entity(const std::string& entityName);

        /**
         * @brief Gets a component by its ID.
         * @param id The ID of the component to retrieve.
         * @return Pointer to the component with the specified ID, or nullptr if not found.
         */
        ConsoleGraphX_Internal::Component* GetComponentByID(int id);

        /**
         * @brief Removes a component by its ID.
         * @param id The ID of the component to be removed.
         */
        std::unordered_map<std::type_index, std::unique_ptr<ConsoleGraphX_Internal::Component>>::iterator RemoveComponentById(int id);

        /**
         * @brief Removes a component by a pointer reference.
         * @param component The component to be removed.
         */
        std::unordered_map<std::type_index, std::unique_ptr<ConsoleGraphX_Internal::Component>>::iterator RemoveComponentC(ConsoleGraphX_Internal::Component* component);

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
        const std::unordered_map<std::type_index, std::unique_ptr<ConsoleGraphX_Internal::Component>>& GetComponents() const;
        const std::unordered_map<std::type_index, std::unique_ptr<ConsoleGraphX_Internal::Component>>& GetScripts() const;

        /**
         * @brief Gets the world position of this entity by combining its local position with that of its children.
         * @return The world position as a Vector3.
         */
        const Vector3 GetPosition() const;

        /**
         * @brief Gets the unique ID of the entity.
         * @return The entity's ID.
         */
        long GetId() const;

        /**
         * @brief Creates a clone of the entity.
         * @return Pointer to the cloned entity.
         */
        Entity* CloneEntity();

        /**
         * @brief Creates a clone of the entity with a specified spread for randomizing the position.
         * @param minSpread The minimum spread for randomizing the position.
         * @param maxSpread The maximum spread for randomizing the position.
         * @return Pointer to the cloned entity.
         */
        Entity* CloneEntity(Vector3 minSpread, Vector3 maxSpread);

        /**
         * @brief Adds a new component of type T to the entity.
         * @tparam T The type of component to be added.
         * @tparam Args Variadic template for component constructor arguments.
         * @param args The arguments to be forwarded to the component constructor.
         * @return Pointer to the added component.
         */
        template <typename T, typename... Args>
        T* AddComponent(Args&&... args) {
            static_assert(std::is_base_of<ConsoleGraphX_Internal::Component, T>::value, "The passed type must be derived from Component.");

            // Create a new instance of the component with arguments and add it to the components map
            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            T* componentPtr = component.get();
            std::type_index index = typeid(T);
            this->_AddComponent(index, std::move(component));

            return componentPtr;
        }

        /**
         * @brief Removes a component of type T from the entity.
         * @tparam T The type of component to be removed.
         */
        template <typename T>
        void RemoveComponent() {
            std::type_index type = typeid(T);
            auto it = _m_components.find(type);
            // If the component is not present in the components map, check in the scripts map
            if (it == _m_components.end())
                it = _m_scripts.find(type);
            // If the component is not present in either map, return early
            if (it == _m_components.end() && it == _m_scripts.end())
                return;

            this->_RemoveComponent(it->first, it->second.get());
        }


        /**
         * @brief Checks if the entity has a component of type T.
         * @tparam T The type of component to check for.
         * @return True if the entity has the specified component, false otherwise.
         */
        template <typename T>
        bool HasComponent() const {
            return _m_components.find(typeid(T)) != _m_components.end() || _m_scripts.find(typeid(T) != _m_scripts.end());
        }

        /**
         * @brief Gets a pointer to the component of type T.
         * @tparam T The type of component to retrieve.
         * @return Pointer to the component of type T, or nullptr if not found.
         */
        template <typename T>
        T* GetComponent() const {
            std::type_index type = typeid(T);

            // Check if the component is present in the components map
            auto compIt = _m_components.find(type);
            if (compIt != _m_components.end()) {
                return static_cast<T*>(compIt->second.get());
            }

            // Check if the component is present in the scripts map
            auto scriptIt = _m_scripts.find(type);
            if (scriptIt != _m_scripts.end()) {
                return static_cast<T*>(scriptIt->second.get());
            }

            // Component not found in either map
            return nullptr;
        }


        /**
         * @brief Calculates the hash value of the entity.
         * @return The hash value of the entity.
         */
        size_t hash() const;

        /**
         * @brief Equality operator for comparing entities.
         * @param other The entity to compare with.
         * @return True if the entities are equal, false otherwise.
         */
        bool operator==(const Entity& other) const;

        /**
         * @brief Inequality operator for comparing entities.
         * @param other The entity to compare with.
         * @return True if the entities are not equal, false otherwise.
         */
        bool operator!=(const Entity& other) const;
    };
};
