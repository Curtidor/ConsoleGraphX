#include "PCH_CGX.h"
#include <cassert>
#include "Engine\Systems\scene_system.h"
#include "Engine\Core\entity.h"
#include "Engine\Scene\scene.h"
#include "Engine\Resources\resource_manager.h"

namespace ConsoleGraphX
{
    void SceneSystem::RegisterScene(std::unique_ptr<Scene> scene)
    {
        _m_scenes[scene->GetSceneName()] = std::move(scene);
    }

    void SceneSystem::DeregisterScene(const std::string& name)
    {
        auto it = _m_scenes.find(name);

        if (name == _s_activeScene->GetSceneName())
            _s_activeScene = nullptr;


        DeleteScene(name);
    }

    void SceneSystem::LoadScene(const std::string& name)
    {
        if (!IsSceneRegistered(name))
        {
            throw std::runtime_error("Scene must be registered");
        }

        // just incase there isnt an already active scene 
        if (_s_activeScene != nullptr)
        {
            _s_activeScene->Destroy();
        }

        // if we make it to here we can guarantee that name is in _m_scenes allow for a direct "index"
        _s_activeScene = _m_scenes[name].get();
        _s_activeScene->Initialize();

        ConsoleGraphX_Internal::ResourceManager::SetActiveManager(&_s_activeScene->GetResourceManager());
    }

    bool SceneSystem::IsSceneRegistered(const std::string& name)
    {
        return _m_scenes.find(name) != _m_scenes.end();
    }

    Scene* SceneSystem::GetActiveScene()
    {
        return _s_activeScene;
    }

    ConsoleGraphX_Internal::ResourceManager& SceneSystem::GetActiveResourceManager()
    {
        return _s_activeScene->GetResourceManager();
    }

    void SceneSystem::DeleteScene(const std::string& name)
    {
        auto it = _m_scenes.find(name);
        
        if (it == _m_scenes.end())
            return;

        Scene& scene = *it->second;
        scene.Destroy();

        _m_scenes.erase(it);
    }

    SceneSystem::SceneSystem()
    {
    }

    SceneSystem::~SceneSystem()
    {
    }

    void SceneSystem::ShutDown()
    {
        while (!_m_scenes.empty())
        {
            DeleteScene(_m_scenes.begin()->first);

            // DeleteScene erases the passed in scene from the map so
            // begin points to a different scene each iteration
        }
    }

};


