#include "CGXPCH.h"
#include <cassert>
#include "scene_system.h"
#include "entity.h"
#include "scene.h"
#include "resource_manager.h"

namespace ConsoleGraphX
{
    void SceneSystem::RegisterScene(Scene* scene)
    {
        _m_scenes[scene->GetSceneName()] = scene;
    }

    void SceneSystem::DeregisterScene(const std::string& name)
    {
        auto it = _m_scenes.find(name);

        if (name == _s_activeScene->GetSceneName())
            _s_activeScene = nullptr;

        DeleteScene(name);
        if (it != _m_scenes.end())
        {
            _m_scenes.erase(it);
        }
    }

    void SceneSystem::LoadScene(const std::string& name)
    {
        if (!IsSceneRegistered(name))
        {
            throw std::runtime_error("Scene must be registered");
        }
        // if we make it to here we can guarantee that name is in _m_scenes allow for a direct "index"
        _s_activeScene = _m_scenes[name];
        _s_activeScene->Initialize();

        ConsoleGraphX_Internal::ResourceManager::SetActiveManager(&_s_activeScene->GetResourceManager());
    }

    void SceneSystem::DeleteScene(const std::string& name)
    {
        auto it = _m_scenes.find(name);
        if (it != _m_scenes.end())
        {
            Scene& scene = *it->second;
            for (Entity entity : scene.GetEntities())
            {
                scene.DeregisterEntity(entity);
            }
        }
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

    SceneSystem& SceneSystem::Instance()
    {
        assert(_s_instance);

        return *_s_instance;
    }

    void SceneSystem::Initialize()
    {
        assert(!_s_instance);

        _s_instance = new SceneSystem();
    }

    void SceneSystem::ShutDown()
    {
        for (auto it = _m_scenes.end(); it != _m_scenes.begin(); )
        {
            DeleteScene(it->first);
            --it; // Decrement first, because .end() is past-the-end.
            it = _m_scenes.erase(it); // erase returns the next valid iterator, but in reverse we need to decrement manually.
        }

        delete _s_instance;
    }

    const std::unordered_map<std::string, Scene*>& SceneSystem::GetScenes()
    {
        return _m_scenes;
    }
};


