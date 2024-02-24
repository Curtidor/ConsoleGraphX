#pragma once
#include <unordered_set>
#include "entity.h"

class Scene
{
private:
    const std::string _m_scene_name;
    std::unordered_set<Entity*> _m_entities;

public:
    Scene(std::string name);

    virtual void Initialize() = 0;

    Entity* RegisterEntity(std::string name = "");
    
    void RegisterEntity(Entity* entity);
    void DeregisterEntity(Entity* entity);
    
    bool InScene(Entity* entity);

    const std::unordered_set<Entity*>& GetEntities();
    const std::string& GetSceneName();
};
