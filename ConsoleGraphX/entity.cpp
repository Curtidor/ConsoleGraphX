#include "entity.h"
#include <memory>
#include <string>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include "component.h"
#include "dispatcher.h"
#include "random_numbers.h"
#include "transform.h"
#include "vector3.h"

namespace ConsoleGraphX
{
    long Entity::_s_totalEntities(0);

    Entity::Entity() : m_name(""), _m_parent(nullptr), m_id(_s_totalEntities++)
    {
        this->AddComponent<Transform>();
    }

    Entity::Entity(const std::string& entityName) : m_name(entityName), _m_parent(nullptr), m_id(_s_totalEntities++)
    {
        this->AddComponent<Transform>();
    }

    void Entity::_CloneComponents(Entity* spawnedEntity) const
    {
        for (const auto& componentPair : this->GetComponents())
        {
            // spawner's can't spawn spawner's and the entity class is responsible for adding transform so there's no need to clone a new one
            if (componentPair.second->GetID() == ComponentID::transform)
                continue;

            auto clonedComponent = std::unique_ptr<ConsoleGraphX_Internal::Component>(componentPair.second->Clone());
            spawnedEntity->_AddComponent(componentPair.first, std::move(clonedComponent));
        }

        for (const auto& scriptPair : this->GetScripts())
        {
            auto clonedScript = std::unique_ptr<ConsoleGraphX_Internal::Component>(scriptPair.second->Clone());
            spawnedEntity->_AddComponent(scriptPair.first, std::move(clonedScript));
           
            ConsoleGraphX_Internal::Dispatcher<Entity*>::Notify("RunTimeScriptAddition", spawnedEntity);
        }
    }

    std::unordered_map<std::type_index, std::unique_ptr<ConsoleGraphX_Internal::Component>>::iterator Entity::_RemoveScript(std::type_index index, ConsoleGraphX_Internal::Component* script)
    {
        auto it = _m_scripts.find(index);
        if (it != _m_scripts.end())
        {
            return _m_scripts.erase(it);
        }

        // Script not found, return end iterator
        return _m_scripts.end();
    }

    std::unordered_map<std::type_index, std::unique_ptr<ConsoleGraphX_Internal::Component>>::iterator Entity::_RemoveComponent(std::type_index index, ConsoleGraphX_Internal::Component* comp)
    {
        bool isScript = comp->GetID() == ComponentID::script;

        std::string componentName = isScript ? "struct ConsoleGraphX::Script" : index.name();
        ConsoleGraphX_Internal::Dispatcher<Entity*>::Notify("RemoveComponent" + componentName, this);
            
        if (!isScript)
        {
            auto it = _m_components.find(index);
            if (it != _m_components.end())
            {
                return _m_components.erase(it);
            }
            return _m_components.end();
        }

        return this->_RemoveScript(index, comp);
    }

    void Entity::_AddComponent(std::type_index index, std::unique_ptr<ConsoleGraphX_Internal::Component> comp)
    {
        std::string componentName = (comp->GetID() == ComponentID::script) ? "struct ConsoleGraphX::Script" : index.name();
        
        if (comp->GetID() == ComponentID::script)
        {
            _m_scripts[index] = std::move(comp);
        }
        else
        {
            _m_components[index] = std::move(comp);
        }

        ConsoleGraphX_Internal::Dispatcher<Entity*>::Notify("AddComponent" + componentName, this);
    }

    std::unordered_map<std::type_index, std::unique_ptr<ConsoleGraphX_Internal::Component>>::iterator Entity::RemoveComponentById(int id)
    {
        // TODO: log a warning if this function is used with a script id it will just remove the first found script
        for (auto it = _m_components.begin(); it != _m_components.end(); ++it)
        {
            if (it->second->GetID() == id)
            {
                return this->_RemoveComponent(it->first, it->second.get());
            }
        }

        return id == ComponentID::script ? _m_scripts.end() : _m_components.end();
    }

    ConsoleGraphX_Internal::Component* Entity::GetComponentByID(int id)
    {
        // TODO: log a warning if this function is used with a script id it will just return the first found script
        for (const auto& component_pair : _m_components)
        {
            if (component_pair.second->GetID() == id)
            {
                return component_pair.second.get();
            }
        }
        return nullptr;
    }

    const Vector3 Entity::GetPosition() const
    {
        return this->GetComponent<Transform>()->GetPosition();
    }

    Entity* Entity::CloneEntity()
    {
        return Entity::CloneEntity(Vector3(0, 0, 0), Vector3(0, 0, 0));
    }

    Entity* Entity::CloneEntity(Vector3 minSpread, Vector3 maxSpread)
    {
        Entity* spawnedEntity = new Entity(this->m_name);
        this->_CloneComponents(spawnedEntity);

        float x = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.x, maxSpread.x);
        float y = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.y, maxSpread.y);
        float z = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.z, maxSpread.z);

        Vector3 prefabPosition = this->GetComponent<Transform>()->GetPosition();
        Vector3 spawnPosition = Vector3(x, y, z) + prefabPosition;
        spawnedEntity->GetComponent<Transform>()->SetPosition(spawnPosition);

        ConsoleGraphX_Internal::Dispatcher<Entity*>::Notify("EntityCreation", spawnedEntity);

        return spawnedEntity;
    }

    std::unordered_map<std::type_index, std::unique_ptr<ConsoleGraphX_Internal::Component>>::iterator Entity::RemoveComponentC(ConsoleGraphX_Internal::Component* component) {
        std::type_index type = typeid(*component);
        return this->_RemoveComponent(type, component);
    }

    const std::unordered_map<std::type_index, std::unique_ptr<ConsoleGraphX_Internal::Component>>& Entity::GetComponents() const
    {
        return this->_m_components;
    }

    const std::unordered_map<std::type_index, std::unique_ptr<ConsoleGraphX_Internal::Component>>& Entity::GetScripts() const
    {
        return this->_m_scripts;
    }

    void Entity::SetParent(Entity* newParent)
    {
        if (_m_parent)
        {
            // Remove from the old parent's children list
            _m_parent->RemoveChild(this);
        }

        _m_parent = newParent;

        if (_m_parent)
        {
            // Add to the new parent's children list
            _m_parent->AddChild(this);
        }
    }

    void Entity::AddChild(Entity* child)
    {
        child->_m_parent = this;
        _m_children.insert(child);
    }

    void Entity::RemoveChild(Entity* child)
    {
        auto it = _m_children.find(child);
        if (it != _m_children.end())
        {
            _m_children.erase(it);
        }
    }

    void Entity::KillEntity()
    {
        ConsoleGraphX_Internal::Dispatcher<Entity*>::Notify("EntityDeletionEvent", this);
        Entity::_s_totalEntities--;
    }

    long Entity::GetId() const
    {
        return m_id;
    }

    size_t Entity::hash() const {
        return std::hash<long>()(m_id);
    }

    bool Entity::operator==(const Entity& other) const {
        return m_id == other.m_id;
    }

    bool Entity::operator!=(const Entity& other) const {
        return m_id != other.m_id;
    }
};
