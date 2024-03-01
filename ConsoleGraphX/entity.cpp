#include "entity.h"

std::atomic<long> Entity::_s_totalEntities(0);

Entity::Entity() : m_name(""), _m_parent(nullptr), m_id(_s_totalEntities++)
{
    this->AddComponent<Transform>();
}

Entity::Entity(const std::string& entityName) : m_name(entityName), _m_parent(nullptr), m_id(_s_totalEntities++)
{
    this->AddComponent<Transform>();
}


Component* Entity::AddComponentClone(Component* comp, std::string& componentName, std::type_index typeIndex)
{
    _m_components[typeIndex] = comp;

    if (comp->GetID() == ComponentID::script)
    {
        componentName = "Struct script";
    }
    std::string event_name = "AddComponent" + componentName;
    Dispatcher<Entity*>::Notify(event_name, this);

    return comp;
}

  

void Entity::RemoveComponentById(int id, bool deleteComponent)
{
    for (std::pair<std::type_index, Component*> componentPair : _m_components)
    {
        if (componentPair.second->GetID() != id)
            continue;

        std::string componentName = componentPair.first.name();

        if (componentPair.second->GetID() == ComponentID::script)
        {
            componentName = "Struct script";
        }

        if (componentPair.second->GetID() == ComponentID::transform)
        {
            throw new std::runtime_error("Entity transforms can not be removed!");
        }

        std::string event_name = "RemoveComponent" + componentName;

        Dispatcher<Entity*>::Notify(event_name, this);

        _m_components.erase(componentPair.first);

        if (deleteComponent)
            delete componentPair.second;
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

   
const Vector3 Entity::GetWorldPosition()
{
    Vector3 world_position = this->GetComponent<Transform>()->GetPosition();
    for (Entity* child : this->_m_children)
    {
        world_position += child->GetWorldPosition();
    }

    return world_position;
}

Entity* Entity::CloneEntity()
{
    return Entity::CloneEntity(Vector3(0, 0, 0), Vector3(0, 0, 0));
}

Entity* Entity::CloneEntity(Vector3 minSpread, Vector3 maxSpread)
{
    Entity* spawnedEntity = new Entity(this->m_name);

    this->CloneComponents(spawnedEntity);

    float x = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.x, maxSpread.x);
    float y = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.y, maxSpread.y);
    float z = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.z, maxSpread.z);

    Vector3 prefabPosition = this->GetComponent<Transform>()->GetPosition();
    Vector3 spawnPosition = Vector3(x, y, z) + prefabPosition;
    spawnedEntity->GetComponent<Transform>()->SetPosition(spawnPosition);

    Dispatcher<Entity*>::Notify("EntityCreation", spawnedEntity);

    return spawnedEntity;
}

void Entity::CloneComponents(Entity* spawnedEntity)
{
    for (std::pair<std::type_index, Component*> componentPair : this->GetComponents())
    {
        // spawner's cant spawn spawner's and the entity class is responsible for adding transform so there's no need to clone a new one
        if (componentPair.second->GetID() == ComponentID::spawner || componentPair.second->GetID() == ComponentID::transform)
            continue;

        Component* clonedComponent = componentPair.second->Clone();
        std::string componentName = componentPair.second->ComponentName();

        spawnedEntity->AddComponentClone(clonedComponent, componentName, componentPair.first);

        if (clonedComponent->GetID() == ComponentID::script)
            Dispatcher<Entity*>::Notify("RunTimeScriptAddition", spawnedEntity);
    }
}

void Entity::RemoveComponentC(Component* component) {
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

  
const std::unordered_map<std::type_index, Component*> Entity::GetComponents()
{
    return this->_m_components;
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

