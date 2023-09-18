#include "scene.h"

void Scene::RegisterEntity(Entity* entity)
{
    entities.insert(entity);

    for (const auto& componentPair : entity->GetComponents())
    {
        std::string typeName = componentPair.first.name();
        std::string eventName = "AddComponent" + typeName;
        Dispatcher<Entity*>::Notify(eventName, entity);
    }
}

void Scene::DeregisterEntity(Entity* entity)
{
    auto it = entities.find(entity);

    if (it != entities.end())
    {
        for (const auto& componentPair : entity->GetComponents())
        {
            entity->RemoveComponent(componentPair.second);
        }

        entities.erase(it);
    }
}

bool Scene::InScene(Entity* entity)
{
    return entities.find(entity) != entities.end();
}

const std::unordered_set<Entity*>& Scene::GetEntities()
{
    return entities;
}
