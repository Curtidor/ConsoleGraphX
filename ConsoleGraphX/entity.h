#pragma once
#include <unordered_set>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>
#include "component_system.h"
#include "transform.h"
#include "dispatcher.h"
#include "game_object.h"

/**
 * @brief A class representing an entity in the entity-component system (ECS).
 */
class Entity 
{
private:
    std::string name;

    Entity* parent;
    std::unordered_set<Entity*> children;

    std::unordered_map<std::type_index, Component*> components;

public:
    /**
     * @brief Constructs an entity with an optional name and adds a Transform component.
     *
     * @param entityName The name of the entity.
     */
    Entity(const std::string& entityName = "") : name(entityName), parent(nullptr)
    {
        this->AddComponent<Transform>();
    }

    /**
     * @brief Sets the parent of this entity.
     *
     * @param newParent The new parent entity.
     */
    void SetParent(Entity* newParent) {
        if (parent) {
            // Remove from the old parent's children list
            parent->RemoveChild(this);
        }

        parent = newParent;

        if (parent) {
            // Add to the new parent's children list
            parent->AddChild(this);
        }
    }

    /**
     * @brief Adds a child entity to this entity.
     *
     * @param child The child entity to add.
     */
    void AddChild(Entity* child) {
        child->parent = this;
        children.insert(child);
    }

    /**
     * @brief Removes a child entity from this entity.
     *
     * @param child The child entity to remove.
     */
    void RemoveChild(Entity* child) {
        auto it = children.find(child);
        if (it != children.end()) {
            children.erase(it);
        }
    }

    /**
     * @brief Adds a component of type T to this entity.
     *
     * @tparam T The type of the component to add.
     * @tparam Args The types of arguments to pass to the component's constructor.
     * @param args The arguments to pass to the component's constructor.
     * @return A pointer to the newly added component.
     */
    template <typename T, typename... Args>
    Component* AddComponent(Args&&... args) {
        std::string component_name = typeid(T).name();

        // Check if the component is registered
        if (!ComponentSystem::IsComponentRegistered(component_name)) {
            std::cerr << "Component " << component_name << " is not registered";
            throw std::runtime_error("");
        }

        // Create a new instance of the component with arguments and add it to the components map
        Component* component = new T(std::forward<Args>(args)...);
        components[typeid(T)] = component;

        return component;
    }

    /**
     * @brief Removes a component of type T from this entity.
     *
     * @tparam T The type of the component to remove.
     */
    template <typename T>
    void RemoveComponent() {
        std::type_index type = typeid(T);

        auto it = components.find(type);
        if (it != components.end()) {
            std::string type_name = type.name();
            std::string event_name = "RemoveComponent" + type_name;
            Dispatcher<Entity*>::Notify(event_name, this);

            delete it->second;
            components.erase(it);
        }
    }

    /**
     * @brief Removes a specific component from this entity.
     *
     * @param component The component to remove.
     */
    void RemoveComponent(Component* component) {
        std::type_index type = typeid(*component);

        auto it = components.find(type);
        if (it != components.end() && it->second == component) {
            std::string type_name = type.name();
            std::string event_name = "RemoveComponent" + type_name;

            Dispatcher<Entity*>::Notify(event_name, this);

            delete it->second;
            components.erase(it);
        }
    }

    /**
     * @brief Checks if this entity has a component of type T.
     *
     * @tparam T The type of the component to check.
     * @return True if the entity has the component, false otherwise.
     */
    template <typename T>
    bool HasComponent() {
        return components.find(typeid(T)) != components.end();
    }

    /**
     * @brief Gets a pointer to a component of type T if it exists in this entity.
     *
     * @tparam T The type of the component to retrieve.
     * @return A pointer to the component if found, or nullptr if not found.
     */
    template <typename T>
    T* GetComponent() {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            return static_cast<T*>(it->second);
        }
        return nullptr;
    }

    /**
     * @brief Gets a reference to the components associated with this entity.
     *
     * @return A reference to the components map.
     */
    const std::unordered_map<std::type_index, Component*>& GetComponents()
    {
        return this->components;
    }

    /**
     * @brief Gets the world position of this entity by combining its local position with that of its children.
     *
     * @return The world position as a Vector3.
     */
    const Vector3 GetWorldPosition()
    {
        Vector3 world_position = this->GetComponent<Transform>()->GetPosition();
        for (Entity* child : this->children)
        {
            world_position += child->GetWorldPosition();
        }

        return world_position;
    }
};
