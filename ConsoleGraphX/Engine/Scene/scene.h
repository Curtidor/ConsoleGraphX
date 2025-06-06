#pragma once
#include <unordered_set>
#include <unordered_map>
#include <string>
#include "Engine\Core\entity.h"
#include "Engine\Resources\resource_manager.h"
#include "Engine\Core\Event\event_guard.h"

namespace ConsoleGraphX
{
    class Scene
    {
    private:
        const std::string _m_scene_name;

        std::unordered_set<Entity, Entity::Hash, Entity::Equal> _m_entities;
        std::unordered_map<std::string, size_t> _m_tagIDMap;

    private:
        void _EntityDestroyedEventHandler(size_t id);
    
    public:
        ConsoleGraphX_Internal::ResourceManager  _m_resourceManager;

    public:
        Scene(std::string name);
        ~Scene();

        virtual void Initialize() = 0;

        Entity* RegisterEntityN(std::string name = "");
        Entity* GetEntity(size_t id);
        Entity* GetEntity(const std::string& tag);

        void Destroy();
        void DeregisterEntity(const Entity& entity);

        const std::unordered_set<Entity, Entity::Hash, Entity::Equal>& GetEntities();
        const std::string& GetSceneName();

        ConsoleGraphX_Internal::ResourceManager& GetResourceManager();
    };

};

