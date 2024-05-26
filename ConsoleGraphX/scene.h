#pragma once
#include <unordered_set>
#include <string>
#include "entity.h"

namespace ConsoleGraphX
{
    class Scene
    {
    private:
        const std::string _m_scene_name;
        std::unordered_set<Entity*> _m_entities;
        std::unordered_set<Entity*> _m_entitiesToDelete;

    public:
        Scene(std::string name);
        ~Scene();

        virtual void Initialize() = 0;


        Entity* RegisterEntityN(std::string name = "");
        void RegisterEntity(Entity* entity);

        void DeregisterEntity(Entity* entity);

        void DeleteEntities();

        bool InScene(Entity* entity);

        const std::unordered_set<Entity*>& GetEntities();
        const std::string& GetSceneName();
    };

};

