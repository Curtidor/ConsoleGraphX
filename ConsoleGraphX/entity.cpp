#include <unordered_map>
#include <string>
#include <type_traits>
#include "entity.h"
#include "random_numbers.h"
#include "transform.h"
#include "vector3.h"
#include "component_id.h"
#include "component_manager.h"
#include "script.h"
#include "base_component_pool.h"

namespace ConsoleGraphX_Internal
{
    size_t EntityIDs::GetId()
    {
        size_t id;
        if (!_s_recycledIds.empty())
        {
            id = _s_recycledIds.front();
            _s_recycledIds.pop();
        }
        else
        {
            id = _s_currentID++;
        }

        return id;
    }

    void EntityIDs::RecycleId(ConsoleGraphX::Entity& entity)
    {
        _s_recycledIds.push(entity.m_id);
    }
};

namespace ConsoleGraphX
{

    Entity::Entity() : m_id(ConsoleGraphX_Internal::EntityIDs::GetId()), m_tag(""), _m_parent(nullptr)
    {
        m_tag = std::to_string(m_id);
        AddComponent<Transform>();
    }

    Entity::Entity(int id) : m_id(id), m_tag(std::to_string(id)), _m_parent(nullptr)
    {
        AddComponent<Transform>();
    }

    Entity::Entity(int id, const std::string& tag) : m_id(id), m_tag(tag), _m_parent(nullptr)
    {
        AddComponent<Transform>();
    }

    Entity::~Entity()
    {
       // DestroyEntity();
    }

    void Entity::Clone(Entity& entity)
    {
        Clone(entity, Vector3(), Vector3());
    }

    void Entity::Clone(Entity& entity, const Vector3& minSpread, const Vector3& maxSpread)
    {
        ConsoleGraphX_Internal::ComponentManager compManager = ConsoleGraphX_Internal::ComponentManager::Instance();
        for (const auto& componentIdIndexPair : _m_componentIdToIndexMap)
        {
            if (componentIdIndexPair.first == ConsoleGraphX_Internal::GenComponentID::Get<Transform>())
            {
                continue;
            }

            ConsoleGraphX_Internal::BaseComponentPool* pool = compManager.GetComponentPoolFromId(componentIdIndexPair.first);

            ConsoleGraphX_Internal::ComponentIndex clonedComponentIndex;

            if (auto* spritePool = dynamic_cast<ConsoleGraphX_Internal::ComponentPoolSprite*>(pool))
            {
                // If the cast succeeds, clone the component with transform for Sprite
                clonedComponentIndex = spritePool->CloneComponentWithTransform(
                    componentIdIndexPair.second, entity._m_componentIdToIndexMap[ConsoleGraphX_Internal::GenComponentID::Get<Transform>()]
                );
            }
            else
            {
                clonedComponentIndex = pool->CloneComponent(componentIdIndexPair.second);
            }

            // insert the cloned component index into the new entity's map
            entity._m_componentIdToIndexMap.insert({ componentIdIndexPair.first, clonedComponentIndex });
        }


        ConsoleGraphX_Internal::ComponentPoolScript* scriptPool = static_cast<ConsoleGraphX_Internal::ComponentPoolScript*>(compManager.GetComponentPoolFromId(ConsoleGraphX_Internal::GenComponentID::Get<Script>()));
        for (const auto& scriptIdIndexPair : _m_scriptIdToIndexes)
        {
            ConsoleGraphX_Internal::ComponentIndex clonedComponentIndex = scriptPool->CloneComponentWithEntity(scriptIdIndexPair.second, &entity);

            entity._m_scriptIdToIndexes.insert({ scriptIdIndexPair.first, clonedComponentIndex });
        }


        float x = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.x, maxSpread.x);
        float y = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.y, maxSpread.y);
        float z = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.z, maxSpread.z);

        Vector3 prefabPosition = GetTransform()->GetLocalPosition();
        Vector3 spawnPosition = Vector3(x, y, z) + prefabPosition;
        
        entity.GetTransform()->SetPosition(spawnPosition);
    }

    const std::unordered_map<ConsoleGraphX_Internal::ComponentID, ConsoleGraphX_Internal::ComponentIndex>& Entity::GetComponents() const
    {
        return _m_componentIdToIndexMap;
    }

    const std::unordered_map<ConsoleGraphX_Internal::ComponentID, ConsoleGraphX_Internal::ComponentIndex>& Entity::GetScripts() const
    {
        return _m_scriptIdToIndexes;
    }

    void Entity::SetParent(Entity* newParent)
    {
        if (_m_parent)
        {
            _m_parent->RemoveChild(this);
        }

        _m_parent = newParent;

        if (_m_parent)
        {
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
        DestroyEntity();
        ConsoleGraphX_Internal::EntityIDs::RecycleId(*this);
    }

    size_t Entity::GetId() const
    {
        return m_id;
    }

    Transform* Entity::GetTransform() 
    {
        return this->GetComponent<Transform>();
    }

    void Entity::_CheckComponentExists(ConsoleGraphX_Internal::ComponentID componentId, const std::unordered_map<ConsoleGraphX_Internal::ComponentID, ConsoleGraphX_Internal::ComponentIndex>& indexMap)
    {
        #ifdef _DEBUG
        if (indexMap.find(componentId) != indexMap.end()) {
            throw std::runtime_error("Component already exists.");
        }
        #endif
    }

    size_t Entity::Hash::operator()(const Entity& entity) const
    {
        return std::hash<int>()(entity.m_id);
    }

    size_t Entity::Hash::operator()(int id) const
    {
        return std::hash<int>()(id);
    }

    bool Entity::Equal::operator()(const Entity& lhs, const Entity& rhs) const
    {
        return lhs.m_id == rhs.m_id && lhs.m_tag == rhs.m_tag;
    }

    bool Entity::Equal::operator()(const Entity& entity, int id) const
    {
        return entity.m_id == id;
    }

    bool Entity::Equal::operator()(int id, const Entity& entity) const
    {
        return id == entity.m_id;
    }

    bool Entity::operator!=(const Entity& other) const 
    {
        return m_id != other.m_id;
    }
};

