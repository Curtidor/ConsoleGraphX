#include "CGXPCH.h"
#include "scene.h"
#include "entity.h"
#include "resource_manager.h"

namespace ConsoleGraphX
{
    Scene::Scene(std::string name)
        : _m_scene_name(name), _m_resourceManager(ConsoleGraphX_Internal::ResourceManager())
    {}

    Scene::~Scene()
    {
        _m_entities.clear();
    }

    Entity* Scene::RegisterEntityN(std::string name)
    {

        auto result = _m_entities.insert(Entity(_m_resourceManager));

        // check if the insertion was successful
        if (!result.second)
        {
            throw std::runtime_error("Entity with the same ID already exists.");
        }

        // NOTE TO FUTURE ME: Casting away const-ness is valid here because the `Entity` objects
        // in the scene are mutable and intended to be modified. The `const_iterator`
        // returned by `emplace` is a `const_iterator` to indicate that it is safe to
        // read from, but since `Entity` objects are not actually const, removing
        // the const qualifier to obtain a modifiable reference is appropriate in this case.
        Entity& insertedEntity = const_cast<Entity&>(*result.first);

        if (!insertedEntity.m_tag.empty())
        {
            _m_tagIDMap.emplace(insertedEntity.m_tag, insertedEntity.m_id);
        }

        return &insertedEntity;
    }


    void Scene::DeregisterEntity(Entity& entity)
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

    Entity* Scene::GetEntity(int id)
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

