#include "scene.h"

Scene::Scene(std::string name)
    : _m_scene_name(name)
{}

void Scene::RegisterEntity(Entity* entity)
{
    _m_entities.insert(entity);

    for (auto& componentPair : entity->GetComponents())
    {
        std::string typeName = componentPair.first.name();
        std::string eventName = "AddComponent" + typeName;
        Dispatcher<Entity*>::Notify(eventName, entity);
    }
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