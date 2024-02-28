#pragma once
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include "transform.h"
#include "dispatcher.h"

/**
 * @brief A class representing an entity in the entity-component system (ECS).
 */
class Entity
{
private:
    const std::string _m_name;

    Entity* parent;

    std::unordered_set<Entity*> _m_children;
    std::unordered_map<std::type_index, Component*> _m_components;

public:
    Entity(const std::string& entityName = "") : _m_name(entityName), parent(nullptr)
    {
        this->AddComponent<Transform>();
    }

    template <typename ComponentType>
    Component* AddScript() {
        static_assert(std::is_base_of<Component, ComponentType>::value, "The passed type must be derived from Component.");

        // Create a new instance of the component with arguments and add it to the components map
        Component* component = new ComponentType(this);
        _m_components[typeid(ComponentType)] = component;

        const std::string event_name = "AddScript";
        Dispatcher<Entity*>::Notify(event_name, this);

        return component;
    }

    void RemoveScript()
    {
        const std::string event_name = "RemoveScript";
        Dispatcher<Entity*>::Notify(event_name, this);

        this->RemoveComponent(GetComponentByID(ComponentID::script));
    }

    template <typename ComponentType, typename... Args>
    Component* AddComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, ComponentType>::value, "The passed type must be derived from Component.");

        std::string componentName = typeid(ComponentType).name();

        // Create a new instance of the component with arguments and add it to the components map
        Component* component = new ComponentType(std::forward<Args>(args)...);
        _m_components[typeid(ComponentType)] = component;

        if (component->GetID() == ComponentID::script)
        {
            componentName = "Struct script";
        }

        std::string event_name = "AddComponent" + componentName;
        Dispatcher<Entity*>::Notify(event_name, this);

        return component;
    }

    template <typename ComponentType>
    void RemoveComponent() {
        std::type_index type = typeid(ComponentType);

        auto it = _m_components.find(type);
        if (it != _m_components.end()) {
            std::string type_name = type.name();
            std::string event_name = "RemoveComponent" + type_name;
            Dispatcher<Entity*>::Notify(event_name, this);

            delete it->second;
            _m_components.erase(it);
        }
    }

    template <typename ComponentType>
    bool HasComponent() {
        return _m_components.find(typeid(ComponentType)) != _m_components.end();
    }

    template <typename ComponentType>
    ComponentType* GetComponent() {
        auto it = _m_components.find(typeid(ComponentType));
        if (it != _m_components.end()) {
            return static_cast<ComponentType*>(it->second);
        }
        return nullptr;
    }

    Component* GetComponentByID(int _m_id)
    {
        for (std::pair<std::type_index, Component*> component_pair : _m_components)
        {
            if (component_pair.second->GetID() == _m_id)
            {
                return component_pair.second;
            }
        }

        return nullptr;
    }

    /**
    * @brief Gets a reference to the components associated with this entity.
    *
    * @return A reference to the components map.
    */
    const std::unordered_map<std::type_index, Component*> GetComponents()
    {
        return this->_m_components;
    }

    /**
     * @brief Gets the world position of this entity by combining its local position with that of its children.
     *
     * @return The world position as a Vector3.
     */
    const Vector3 GetWorldPosition()
    {
        Vector3 world_position = this->GetComponent<Transform>()->GetPosition();
        for (Entity* child : this->_m_children)
        {
            world_position += child->GetWorldPosition();
        }

        return world_position;
    }

    void RemoveComponent(Component* component) {
        std::type_index type = typeid(*component);

        auto it = _m_components.find(type);
        if (it != _m_components.end() && it->second == component) {
            std::string componentName = type.name();

            if (component->GetID() == ComponentID::script)
            {
                componentName = "Struct script";
            }

            std::string event_name = "RemoveComponent" + componentName;

            Dispatcher<Entity*>::Notify(event_name, this);

            delete it->second;
            _m_components.erase(it);
        }
    }

    void SetParent(Entity* newParent) 
    {
        if (parent) 
        {
            // Remove from the old parent's children list
            parent->RemoveChild(this);
        }

        parent = newParent;

        if (parent) 
        {
            // Add to the new parent's children list
            parent->AddChild(this);
        }
    }

    void AddChild(Entity* child) 
    {
        child->parent = this;
        _m_children.insert(child);
    }


    void RemoveChild(Entity* child) 
    {
        auto it = _m_children.find(child);
        if (it != _m_children.end()) {
            _m_children.erase(it);
        }
    }

    void KillEntity()
    {
        Dispatcher<Entity*>::Notify("EntityDeletionEvent", this);
    }
};