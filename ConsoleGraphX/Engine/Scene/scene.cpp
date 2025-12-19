#include "PCH_CGX.h"
#include "Engine\Scene\scene.h"
#include "Engine\Core\entity.h"
#include "Engine\Resources\resource_manager.h"

namespace ConsoleGraphX
{
    Scene::Scene(std::string name)
        : _m_scene_name(name), _m_resourceManager(ConsoleGraphX_Internal::ResourceManager())
    {
        EntityDestroyedEvent.AddListener(this, &Scene::_EntityDestroyedEventHandler);
    }

    Scene::~Scene()
    {
        _m_entities.clear();
    }

    Entity* Scene::RegisterEntityN(std::string name)
    {
        auto result = _m_entities.emplace(std::move(Entity(&_m_resourceManager)));

        if (!result.second)
        {
            throw std::runtime_error("Entity with the same ID already exists.");
        }

        Entity& insertedEntity = const_cast<Entity&>(*result.first);


        if (!insertedEntity.m_tag.empty())
        {
            _m_tagIDMap.emplace(insertedEntity.m_tag, insertedEntity.m_id);
        }

        return &insertedEntity;
    }


    void Scene::DeregisterEntity(const Entity& entity)
    {
        auto itEntity = _m_entities.find(entity);
        if (itEntity == _m_entities.end())
        {
            return;
        }

        auto itId = _m_tagIDMap.find(entity.m_tag);
        if (itId != _m_tagIDMap.end())
        {
            _m_tagIDMap.erase(itId);
        }


        _m_entities.erase(itEntity);
    }

    void Scene::CleanUpDeadEntities()
    {
        for(size_t id : _m_entitiesToKill)
        {
            DeregisterEntity(*GetEntity(id));
        }
    }

    void Scene::_EntityDestroyedEventHandler(size_t id)
    {
        _m_entitiesToKill.push_back(id);
    }

    void Scene::Destroy()
    {
        // copy entities to a temp list to avoid modifying the set while iterating
        std::vector<Entity*> entitiesToKill;
        entitiesToKill.reserve(_m_entities.size());

        for (const auto& entity : _m_entities)
        {
            entitiesToKill.push_back(const_cast<Entity*>(&entity));
        }

        // kill each entity (which removes them from the set internally)
        for (Entity* entity : entitiesToKill)
        {
            entity->KillEntity(); // killEntity will remove it from _m_entities
        }

        _m_entities.clear();     
        _m_tagIDMap.clear();
    }


    Entity* Scene::GetEntity(size_t id)
    {
        auto it = _m_entities.find(id);

        if (it == _m_entities.end())
        {
            return nullptr;
        }

        return  &const_cast<Entity&>(*it);
    }

    Entity* Scene::GetEntity(const std::string& tag)
    {
        auto itID = _m_tagIDMap.find(tag);

        if (itID == _m_tagIDMap.end())
        {
            return nullptr;
        }

        auto itEntity = _m_entities.find(itID->second);

        if (itEntity == _m_entities.end())
        {
            return nullptr;
        }

        return  &const_cast<Entity&>(*itEntity);
    }

    const std::unordered_set<Entity, Entity::Hash, Entity::Equal>& Scene::GetEntities()
    {
        return _m_entities;
    }

    ConsoleGraphX_Internal::ResourceManager& Scene::GetResourceManager()
    {
        return _m_resourceManager;
    }

    const std::string& Scene::GetSceneName()
    {
        return _m_scene_name;
    }
};

