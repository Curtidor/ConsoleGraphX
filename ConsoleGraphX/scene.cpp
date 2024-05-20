#include <string>
#include <unordered_set>
#include "scene.h"
#include "entity.h"

namespace ConsoleGraphX
{
    Scene::Scene(std::string name)
        : _m_scene_name(name)
    {}

    Scene::~Scene()
    {
        _m_entitiesToDelete.insert(_m_entities.begin(), _m_entities.end());

        // DeleteEntities also removes the entity from _m_entities
        DeleteEntities();
    }

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
                auto& components = entity->GetComponents();
                for (auto itComponents = components.begin(); itComponents != components.end();)
                {
                    itComponents = entity->RemoveComponentC(itComponents->second.get());
                }

                auto& scripts = entity->GetScripts();
                for(auto itScripts = scripts.begin(); itScripts != scripts.end();)
                {
                    itScripts = entity->RemoveComponentC(itScripts->second.get());
                }

                // Erase the entity from the scene
                _m_entities.erase(it);

                // Delete the entity itself
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
};

