#pragma once
#include <unordered_set>
#include <unordered_map>
#include <string>
#include "entity.h"

namespace ConsoleGraphX
{
    class Scene
    {
    private:
        const std::string _m_scene_name;

        std::unordered_set<Entity, Entity::Hash, Entity::Equal> _m_entities;
        std::unordered_set<Entity*> _m_entitiesToDelete;

        std::unordered_map<std::string, int> _m_tagIDMap;

    public:
        Scene(std::string name);
        ~Scene();

        virtual void Initialize() = 0;

        Entity* RegisterEntityN(std::string name = "");
        Entity* GetEntity(int id);
        Entity* GetEntity(const std::string& tag);

        void DeregisterEntity(Entity& entity);


        const std::unordered_set<Entity>& GetEntities();
        const std::string& GetSceneName();
    };

};

