#include "scene_system.h"

Scene* SceneSystem::_s_active_scene;
std::unordered_map<std::string, Scene*> SceneSystem::_s_scenes;

void SceneSystem::RegisterScene(Scene* scene)
{
    _s_scenes[scene->GetSceneName()] = scene;
}

void SceneSystem::DeregisterScene(const std::string& name)
{
    auto it = _s_scenes.find(name);

    if (name == _s_active_scene->GetSceneName())
        _s_active_scene = nullptr;

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
    _s_active_scene = _s_scenes[name];
    _s_active_scene->Initialize();
}

void SceneSystem::DeleteScene(const std::string& name)
{
    auto it = _s_scenes.find(name);
    if (it != _s_scenes.end())
    {
        Scene* scene = it->second;
        for (Entity* entity : scene->GetEntities())
        {
            scene->DeregisterEntity(entity);
        }
    }
}

bool SceneSystem::IsSceneRegistered(const std::string& name)
{
    return _s_scenes.find(name) != _s_scenes.end();
}

const std::unordered_map<std::string, Scene*>& SceneSystem::GetScenes()
{
    return _s_scenes;
}
