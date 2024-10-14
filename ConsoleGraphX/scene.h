#pragma once
#include <unordered_set>
#include <unordered_map>
#include <string>
#include "entity.h"
#include "resource_manager.h"

namespace ConsoleGraphX
{
    class SceneSystem;

    class Scene
    {
    private:
        const std::string _m_scene_name;

        std::unordered_set<Entity, Entity::Hash, Entity::Equal> _m_entities;
        std::unordered_map<std::string, int> _m_tagIDMap;
    
    protected:
        ConsoleGraphX_Internal::ResourceManager  _m_resourceManager;

    public:
        Scene(std::string name);
        ~Scene();

        virtual void Initialize() = 0;

        Entity* RegisterEntityN(std::string name = "");
        Entity* GetEntity(int id);
        Entity* GetEntity(const std::string& tag);

        void DeregisterEntity(Entity& entity);

        const std::unordered_set<Entity, Entity::Hash, Entity::Equal>& GetEntities();
        const std::string& GetSceneName();

        ConsoleGraphX_Internal::ResourceManager& GetResourceManager();
    };

};

