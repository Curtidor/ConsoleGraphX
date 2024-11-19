#pragma once
#include <unordered_map>
#include <string>
#include "resource_manager.h"
#include "scene.h"

namespace ConsoleGraphX
{
    class SceneSystem
    {
    private:
        Scene* _s_activeScene = nullptr;
        std::unordered_map<std::string, Scene*> _m_scenes;

    public:
        ~SceneSystem();

        void RegisterScene(Scene* scene);
        void DeregisterScene(const std::string& name);
        void LoadScene(const std::string& name);
        void DeleteScene(const std::string& name);

        bool IsSceneRegistered(const std::string& name);

        const std::unordered_map<std::string, Scene*>& GetScenes();

        Scene* GetActiveScene();
        ConsoleGraphX_Internal::ResourceManager& GetActiveResourceManager();
    };

};

