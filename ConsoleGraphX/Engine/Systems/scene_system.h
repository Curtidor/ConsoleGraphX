#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Engine\Resources\resource_manager.h"
#include "Engine\Scene\scene.h"
#include "Engine\Systems\base_system.h"

namespace ConsoleGraphX
{
    class SceneSystem : public BaseSystem
    {
    private:
        Scene* _m_activeScene = nullptr;
        std::string _m_sceneToLoad;
        std::unordered_map<std::string, std::unique_ptr<Scene>> _m_scenes;

    private:
        void LoadSceneImpl(const std::string& name);

    public:
        SceneSystem();
        ~SceneSystem();

        void Initialize() override;
        void Update(float delta_time, SceneSystem& sceneSystem) override;
        void ShutDown();

        void RegisterScene(std::unique_ptr<Scene> scene);
        void DeregisterScene(const std::string& name);
        void LoadScene(const std::string& name);
        void DeleteScene(const std::string& name);

        bool IsSceneRegistered(const std::string& name);

        Scene* GetActiveScene();
        ConsoleGraphX_Internal::ResourceManager& GetActiveResourceManager();
    };

};

