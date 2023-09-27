#include "scene.h"

Scene::Scene(std::string name)
    : _m_scene_name(name)
{}

Entity* Scene::RegisterEntity(std::string name)
{
    Entity* entity = new Entity(name);
    _m_entities.insert(entity);

    return entity;
}

void Scene::RegisterEntity(Entity* entity)
{
    _m_entities.insert(entity);
}

void Scene::DeregisterEntity(Entity* entity)
{
    auto it = _m_entities.find(entity);

    if (it != _m_entities.end())
    {
        for (auto& componentPair : entity->GetComponents())
        {
            entity->RemoveComponent(componentPair.second);
        }

        _m_entities.erase(it);
    }
}

bool Scene::InScene(Entity* entity)
{
    return _m_entities.find(entity) != _m_entities.end();
}

const std::unordered_set<Entity*>& Scene::GetEntities()
{
    return _m_entities;
}

const std::string& Scene::GetSceneName()
{
    return _m_scene_name;
}