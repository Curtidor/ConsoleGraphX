#include "entity.h"
#include <stdexcept>
#include <atomic>
#include <string>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include "component.h"
#include "dispatcher.h"
#include "random_numbers.h"
#include "transform.h"
#include "vector3.h"

std::atomic<long> Entity::_s_totalEntities(0);

Entity::Entity() : m_name(""), _m_parent(nullptr), m_id(_s_totalEntities++)
{
    this->AddComponent<Transform>();
}

Entity::Entity(const std::string& entityName) : m_name(entityName), _m_parent(nullptr), m_id(_s_totalEntities++)
{
    this->AddComponent<Transform>();
}


void Entity::_CloneComponents(Entity* spawnedEntity)
{
    for (std::pair<std::type_index, Component*> componentPair : this->GetComponents())
    {
        // spawner's cant spawn spawner's and the entity class is responsible for adding transform so there's no need to clone a new one
        if (componentPair.second->GetID() == ComponentID::transform)
            continue;

        Component* clonedComponent = componentPair.second->Clone();

        spawnedEntity->_AddComponent(componentPair.first, clonedComponent);
    }

    for (Component* comp: this->GetScripts())
    {
        Component* componentClone = comp->Clone();
        std::type_index index = typeid(componentClone);

        spawnedEntity->_AddComponent(index, componentClone);

        Dispatcher<Entity*>::Notify("RunTimeScriptAddition", spawnedEntity);
    }
}

void Entity::_AddScript(Component* script)
{
    if (script->GetID() != ComponentID::script)
        throw std::runtime_error("script param must be of type Script");

    _m_scripts.insert(script);
}

void Entity::_RemoveScript(Component* script)
{
    auto it = _m_scripts.find(script);

    if (it != _m_scripts.end())
        _m_scripts.erase(it);
}
  
void Entity::_RemoveComponent(std::type_index type, Component* comp)
{
    bool isScript = comp->GetID() == ComponentID::script;

    std::string componentName = isScript ? "struct Script" : type.name();

    Dispatcher<Entity*>::Notify("RemoveComponent" + componentName, this);

    if (isScript)
    {
        this->_RemoveScript(comp);
    }
    else
    {
        auto it = _m_components.find(type);
        if (it != _m_components.end())
            _m_components.erase(it);
    }

    delete comp;
}


void Entity::_AddComponent(std::type_index index, Component* comp)
{
    std::string componentName = (comp->GetID() == ComponentID::script) ? "struct Script" : index.name();

    if (comp->GetID() == ComponentID::script)
    {
        this->_AddScript(comp);
    }
    else
    {
        _m_components[index] = comp;
    }

    Dispatcher<Entity*>::Notify("AddComponent" + componentName, this);
}

void Entity::RemoveComponentById(int id, bool deleteComponent)
{
    for (std::pair<std::type_index, Component*> componentPair : _m_components)
    {
        if (componentPair.second->GetID() == id)
        {
            this->_RemoveComponent(componentPair.first, componentPair.second);
            break;
        }
    }
}


Component* Entity::GetComponentByID(int id)
{
    for (std::pair<std::type_index, Component*> component_pair : _m_components)
    {
        if (component_pair.second->GetID() == id)
        {
            return component_pair.second;
        }
    }

    return nullptr;
}

   
const Vector3 Entity::GetPosition()
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

    Dispatcher<Entity*>::Notify("EntityCreation", spawnedEntity);

    return spawnedEntity;
}

void Entity::RemoveComponentC(Component* component) {
    std::type_index type = typeid(*component);

    this->_RemoveComponent(type, component);
}


  
const std::unordered_map<std::type_index, Component*> Entity::GetComponents()
{
    return this->_m_components;
}

const std::unordered_set<Component*> Entity::GetScripts()
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
    if (it != _m_children.end()) {
        _m_children.erase(it);
    }
}

void Entity::KillEntity()
{
    Dispatcher<Entity*>::Notify("EntityDeletionEvent", this);
    Entity::_s_totalEntities--;
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

