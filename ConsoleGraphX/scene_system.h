#pragma once
#include <unordered_map>
#include <string>
#include "resource_manager.h"
#include "scene.h"
#include "events.h"

namespace ConsoleGraphX
{
    class SceneSystem
    {
    private:
        Scene* _s_activeScene = nullptr;
        std::unordered_map<std::string, std::unique_ptr<Scene>> _m_scenes;

    public:

    public:
        SceneSystem();
        ~SceneSystem();


        void RegisterScene(std::unique_ptr<Scene> scene);
        void DeregisterScene(const std::string& name);
        void LoadScene(const std::string& name);
        void DeleteScene(const std::string& name);

        bool IsSceneRegistered(const std::string& name);

        Scene* GetActiveScene();
        ConsoleGraphX_Internal::ResourceManager& GetActiveResourceManager();
    };

};

