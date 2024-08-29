#include "scene_system.h"
#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "dispatcher.h"
#include "entity.h"
#include "scene.h"

namespace ConsoleGraphX
{
    Scene* SceneSystem::_s_activeScene;
    std::unordered_map<std::string, Scene*> SceneSystem::_s_scenes;

    void SceneSystem::RegisterScene(Scene* scene)
    {
        _s_scenes[scene->GetSceneName()] = scene;

       // ConsoleGraphX_Internal::Dispatcher<Entity*>::RegisterListener("EntityCreation", std::bind(&Scene::RegisterEntity, scene, std::placeholders::_1));
       // ConsoleGraphX_Internal::Dispatcher<Entity*>::RegisterListener("EntityDeletionEvent", std::bind(&Scene::DeregisterEntity, scene, std::placeholders::_1));
    }

    void SceneSystem::DeregisterScene(const std::string& name)
    {
        auto it = _s_scenes.find(name);

        if (name == _s_activeScene->GetSceneName())
            _s_activeScene = nullptr;

        DeleteScene(name);
        if (it != _s_scenes.end())
        {
            _s_scenes.erase(it);
        }
    }

    void SceneSystem::LoadScene(const std::string& name)
    {
        if (!IsSceneRegistered(name))
        {
            throw std::runtime_error("Scene must be registered");
        }
        _s_activeScene = _s_scenes[name];
        _s_activeScene->Initialize();
    }

    void SceneSystem::DeleteScene(const std::string& name)
    {
        auto it = _s_scenes.find(name);
        if (it != _s_scenes.end())
        {
            Scene* scene = it->second;
            for (Entity entity : scene->GetEntities())
            {
                scene->DeregisterEntity(entity);
            }
        }
    }

    bool SceneSystem::IsSceneRegistered(const std::string& name)
    {
        return _s_scenes.find(name) != _s_scenes.end();
    }

    Scene* SceneSystem::GetActiveScene()
    {
        return _s_activeScene;
    }

    const std::unordered_map<std::string, Scene*>& SceneSystem::GetScenes()
    {
        return _s_scenes;
    }
};


