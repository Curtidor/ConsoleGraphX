#pragma once
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <string>
#include <type_traits>
#include "component.h"
#include "vector3.h"

/**
 * @brief A class representing an entity in the entity-component system (ECS).
 */
class Entity
{
private:
    static std::atomic<long> _s_totalEntities;

    Entity* _m_parent;
    std::unordered_set<Entity*> _m_children;
    std::unordered_map<std::type_index, Component*> _m_components;
    std::unordered_set<Component*> _m_scripts;

    void _CloneComponents(Entity* spawnedEntity);
    void _AddScript(Component* script);
    void _RemoveScript(Component* script);
    void _RemoveComponent(std::type_index type, Component* comp);
    void _AddComponent(std::type_index type, Component* comp);
   

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
    Component* GetComponentByID(int id);

    /**
     * @brief Removes a component by its ID.
     * @param id The ID of the component to be removed.
     * @param deleteComponent Flag indicating whether to delete the removed component.
     * @note Using this method with scripts can lead to undefined behavior unless the script has its own id
     */
    void RemoveComponentById(int id, bool deleteComponent = true);

    /**
     * @brief Removes a component by a pointer reference.
     * @param component The component to be removed.
     */
    void RemoveComponentC(Component* component);

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
    const std::unordered_map<std::type_index, Component*> GetComponents();
    const std::unordered_set<Component*> GetScripts();

    /**
     * @brief Gets the world position of this entity by combining its local position with that of its children.
     * @return The world position as a Vector3.
     */
    const Vector3 GetPosition();

    /**
     * @brief Gets the unique ID of the entity.
     * @return The entity's ID.
     */
    long GetId();

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
    Component* AddComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "The passed type must be derived from Component.");
        
        // Create a new instance of the component with arguments and add it to the components map
        Component* component = new T(std::forward<Args>(args)...);
        this->_AddComponent(typeid(T), component);

        return component;
    }

    /**
     * @brief Removes a component of type T from the entity.
     * @tparam T The type of component to be removed.
     * @param deleteComponent Flag indicating whether to delete the removed component.
     */
    template <typename T>
    void RemoveComponent(bool deleteComponent = true) {
        std::type_index type = typeid(T);

        auto it = _m_components.find(type);

        this->_RemoveComponent(it->first, it->second);
    }

    /**
     * @brief Checks if the entity has a component of type T.
     * @tparam T The type of component to check for.
     * @return True if the entity has the specified component, false otherwise.
     */
    template <typename T>
    bool HasComponent() {
        return _m_components.find(typeid(T)) != _m_components.end();
    }

    /**
     * @brief Gets a pointer to the component of type T.
     * @tparam T The type of component to retrieve.
     * @return Pointer to the component of type T, or nullptr if not found.
     */
    template <typename T>
    T* GetComponent() {
        auto it = _m_components.find(typeid(T));
        if (it != _m_components.end()) {
            T* component = dynamic_cast<T*>(it->second);
            if (component) {
                return component;
            }
        }

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
