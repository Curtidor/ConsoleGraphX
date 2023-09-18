#pragma once
#include <unordered_set>
#include "entity.h"

class Scene
{
private:
    std::unordered_set<Entity*> entities;

public:
    void RegisterEntity(Entity* entity);
    void DeregisterEntity(Entity* entity);
    bool InScene(Entity* entity);
    const std::unordered_set<Entity*>& GetEntities();
};
