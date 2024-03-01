#pragma once
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include "transform.h"
#include "dispatcher.h"
#include "random_numbers.h"


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

    void CloneComponents(Entity* spawnedEntity);

public:
    const long m_id;
    const std::string m_name;

    Entity();
    Entity(const std::string& entityName);

    Component* AddComponentClone(Component* comp, std::string& componentName, std::type_index typeIndex);
    Component* GetComponentByID(int id);

    void RemoveComponentById(int id, bool deleteComponent = true);
    void RemoveComponentC(Component* component);
    
    void SetParent(Entity* newParent);
    void AddChild(Entity* child);
    void RemoveChild(Entity* child);

    void KillEntity();
    /**
   * @brief Gets a reference to the components associated with this entity.
   *
   * @return A reference to the components map.
   */
    const std::unordered_map<std::type_index, Component*> GetComponents();

    /**
    * @brief Gets the world position of this entity by combining its local position with that of its children.
    *
    * @return The world position as a Vector3.
    */
    const Vector3 GetWorldPosition();

    long GetId();

    Entity* CloneEntity();
    Entity* CloneEntity(Vector3 minSpread, Vector3 maxSpread);

    template <typename T, typename... Args>
    Component* AddComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "The passed type must be derived from Component.");

        std::string componentName = typeid(T).name();

        // Create a new instance of the component with arguments and add it to the components map
        Component* component = new T(std::forward<Args>(args)...);
        _m_components[typeid(T)] = component;

        if (component->GetID() == ComponentID::script)
        {
            componentName = "Struct script";
        }

        std::string event_name = "AddComponent" + componentName;
        Dispatcher<Entity*>::Notify(event_name, this);

        return component;
    }

    template <typename T>
    void RemoveComponent(bool deleteComponent = true) {
        std::type_index type = typeid(T);

        auto it = _m_components.find(type);

        if (it != _m_components.end()) {

            std::string componentName = type.name();

            if (it->second->GetID() == ComponentID::script)
            {
                componentName = "Struct script";
            }

            if (it->second->GetID() == ComponentID::transform)
            {
                throw new std::runtime_error("Entity transforms can not be removed!");
            }

            std::string eventName = "RemoveComponent" + componentName;
            Dispatcher<Entity*>::Notify(eventName, this);

            if (deleteComponent)
                delete it->second;
            _m_components.erase(it);
        }
    }

    template <typename T>
    bool HasComponent() {
        return _m_components.find(typeid(T)) != _m_components.end();
    }

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


    size_t hash() const;

    bool operator==(const Entity& other) const;

    bool operator!=(const Entity& other) const;
};