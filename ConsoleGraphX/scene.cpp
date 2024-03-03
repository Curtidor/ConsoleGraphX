#include "scene.h"
#include <string>
#include <unordered_set>
#include "component.h"
#include "entity.h"
#include "script.h"

Scene::Scene(std::string name)
    : _m_scene_name(name)
{}

Entity* Scene::RegisterEntityN(std::string name)
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
    this->_m_entitiesToDelete.insert(entity);
}

void Scene::DeleteEntities()
{
    for (Entity* entity : this->_m_entitiesToDelete)
    {
        auto it = _m_entities.find(entity);

        if (it != _m_entities.end())
        {
            for (auto& componentPair : entity->GetComponents())
            {
                entity->RemoveComponentC(componentPair.second);
            }

            for (Component* comp: entity->GetScripts())
            {
                Script* script = static_cast<Script*>(comp);
                entity->RemoveComponentC(script);
            }

            _m_entities.erase(it);

            delete entity;
        }
    }
    this->_m_entitiesToDelete.clear();
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