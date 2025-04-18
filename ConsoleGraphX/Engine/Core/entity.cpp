#include "PCH_CGX.h"
#include "Engine\Core\Event\events.h"
#include "Engine\Core\Utils\random_numbers.h"
#include "Engine\Components\transform.h"
#include "Engine\Components\script.h"
#include "Engine\Resources\resource_id.h"
#include "Engine\Resources\resource_manager.h"
#include "Engine\Resources\Pools\base_resource_pool.h"
#include "Engine\Resources\Pools\component_sprite_pool.h"
#include "Engine\Resources\Pools\component_script_pool.h"
#include "Engine\Core\entity.h"

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

    void EntityIDs::RecycleId(size_t id)
    {
        _s_recycledIds.push(id);
    }
};

namespace ConsoleGraphX
{
    Entity::Entity(int id) : m_id(id), m_tag(""), _m_parent(nullptr), _m_resourceManager(&ConsoleGraphX_Internal::ResourceManager::GetActiveResourceManager())
    {
        m_tag = std::to_string(m_id);
        AddComponent<Transform>();
    }

    Entity::Entity(ConsoleGraphX_Internal::ResourceManager* resourceManager) : m_id(ConsoleGraphX_Internal::EntityIDs::GetId()), m_tag(""), _m_parent(nullptr), _m_resourceManager(resourceManager)
    {
        m_tag = std::to_string(m_id);
        AddComponent<Transform>();
    }

    Entity::Entity(ConsoleGraphX_Internal::ResourceManager* resourceManager, int id) : m_id(id), m_tag(std::to_string(id)), _m_parent(nullptr), _m_resourceManager(resourceManager)
    {
        AddComponent<Transform>();
    }

    Entity::Entity(ConsoleGraphX_Internal::ResourceManager* resourceManager, int id, const std::string& tag) : m_id(id), m_tag(tag), _m_parent(nullptr), _m_resourceManager(resourceManager)
    {
        AddComponent<Transform>();
    }

    Entity::~Entity()
    {}

    Entity::Entity(Entity && other) noexcept
        : _m_parent(other._m_parent),
        _m_resourceManager(other._m_resourceManager),
        _m_children(std::move(other._m_children)),
        _m_componentIdToIndexMap(std::move(other._m_componentIdToIndexMap)),
        _m_scriptIdToIndexes(std::move(other._m_scriptIdToIndexes)),
        m_id(other.m_id),
        m_tag(std::move(other.m_tag))
    {
        other._m_parent = nullptr;
    }

    Entity& Entity::operator=(Entity&& other) noexcept
    {
        if (this != &other)
        {
            _m_parent = other._m_parent;
            _m_resourceManager = other._m_resourceManager;
            _m_children = std::move(other._m_children);
            _m_componentIdToIndexMap = std::move(other._m_componentIdToIndexMap);
            _m_scriptIdToIndexes = std::move(other._m_scriptIdToIndexes);
            //m_id = other.m_id;
            m_tag = std::move(other.m_tag);

            other._m_parent = nullptr;
        }
        return *this;
    }

    void Entity::Clone(Entity& entity)
    {
        Clone(entity, Vector3(), Vector3());
    }

    void Entity::Clone(Entity& entity, const Vector3& minSpread, const Vector3& maxSpread)
    {
        for (const auto& componentIdIndexPair : _m_componentIdToIndexMap)
        {
            if (componentIdIndexPair.first == ConsoleGraphX_Internal::GenResourceID::Get<Transform>())
            {
                continue;
            }

            auto poolVariant = _m_resourceManager->GetResourcePoolFromId(componentIdIndexPair.first);

            ConsoleGraphX_Internal::ResourceIndex clonedComponentIndex = 0;

            std::visit([&](auto& poolWrapper)
                {
                    auto& pool = poolWrapper.get(); // unwrap the std::reference_wrapper

                    using PoolType = std::decay_t<decltype(pool)>;

                    if constexpr (std::is_same_v<PoolType, ConsoleGraphX_Internal::ComponentPoolSprite>)
                    {
                        // clone with transform for Sprite
                        clonedComponentIndex = pool.CloneComponentWithTransform(
                            componentIdIndexPair.second,
                            _m_resourceManager,
                            entity._m_componentIdToIndexMap[ConsoleGraphX_Internal::GenResourceID::Get<Transform>()]
                        );
                    }
                    else
                    {
                        // general case for other pools
                        clonedComponentIndex = pool.CloneResource(componentIdIndexPair.second);
                    }
                }, poolVariant);


            // insert the cloned component index into the new entity's map
            entity._m_componentIdToIndexMap.insert({ componentIdIndexPair.first, clonedComponentIndex });
        }

        ConsoleGraphX_Internal::ComponentPoolScript& scriptPool = _m_resourceManager->GetResourcePool<Script>();
        for (const auto& scriptIdIndexPair : _m_scriptIdToIndexes)
        {
            ConsoleGraphX_Internal::ResourceIndex clonedComponentIndex = scriptPool.CloneComponentWithEntity(scriptIdIndexPair.second, &entity);

            entity._m_scriptIdToIndexes.insert({ scriptIdIndexPair.first, clonedComponentIndex });
        }

        // this is here for simply connivances, and should be moved to a spawner class or something of the sorts
        float x = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.x, maxSpread.x);
        float y = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.y, maxSpread.y);
        float z = RandomNumberGenerator::GenerateRandomFloatInRange(minSpread.z, maxSpread.z);

        Vector3 prefabPosition = GetTransform()->GetLocalPosition();
        Vector3 spawnPosition = Vector3(x, y, z) + prefabPosition;
        
        entity.GetTransform()->SetPosition(spawnPosition);
    }

    const std::unordered_map<ConsoleGraphX_Internal::ResourceID, ConsoleGraphX_Internal::ResourceIndex>& Entity::GetComponents() const
    {
        return _m_componentIdToIndexMap;
    }

    const std::unordered_map<ConsoleGraphX_Internal::ResourceID, ConsoleGraphX_Internal::ResourceIndex>& Entity::GetScripts() const
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
        for(const Entity* child : _m_children)
        {
            child->DestroyEntityResources();
            ConsoleGraphX_Internal::EntityIDs::RecycleId(child->m_id);
        }

        DestroyEntityResources();
        ConsoleGraphX_Internal::EntityIDs::RecycleId(m_id);

        EntityDestroyedEvent.Invoke(m_id);
    }

    size_t Entity::GetId() const
    {
        return m_id;
    }

    Transform* Entity::GetTransform() 
    {
        return this->GetComponent<Transform>();
    }

    void Entity::DestroyEntityResources() const
    {
        _m_resourceManager->DestroyEntityResources(_m_componentIdToIndexMap);
        _m_resourceManager->DestroyEntityResources(_m_scriptIdToIndexes);
    }

    ConsoleGraphX_Internal::ResourceManager& Entity::GetResourceManager() const
    {
        return *_m_resourceManager;
    }

    void Entity::_CheckComponentExists(ConsoleGraphX_Internal::ResourceID componentId, const std::unordered_map<ConsoleGraphX_Internal::ResourceID, ConsoleGraphX_Internal::ResourceIndex>& indexMap)
    {
        // this is here so if we try to double add a component in a debug build we will get an error, as no entity should have
        // two of the same components, other scripts
        #ifdef _DEBUG
        if (indexMap.find(componentId) != indexMap.end())
        {
            throw std::runtime_error("Component already exists.");
        }
        #endif
    }

    size_t Entity::Hash::operator()(const Entity& entity) const
    {
        return std::hash<size_t>()(entity.m_id);
    }

    size_t Entity::Hash::operator()(size_t id) const
    {
        return std::hash<size_t>()(id);
    }

    bool Entity::Equal::operator()(const Entity& lhs, const Entity& rhs) const
    {
        return lhs.m_id == rhs.m_id && lhs.m_tag == rhs.m_tag;
    }

    bool Entity::Equal::operator()(const Entity& entity, int id) const
    {
        return entity.m_id == id;
    }

    bool Entity::Equal::operator()(size_t id, const Entity& entity) const
    {
        return id == entity.m_id;
    }

    bool Entity::operator!=(const Entity& other) const
    {
        return m_id != other.m_id;
    }
};

