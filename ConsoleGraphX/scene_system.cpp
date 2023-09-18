#include "scene_system.h"

Scene* SceneSystem::_s_active_scene;
std::unordered_set<Scene*> SceneSystem::_s_scenes;

void SceneSystem::RegisterScene(Scene* scene)
{
    _s_scenes.insert(scene);
}

void SceneSystem::DeregisterScene(Scene* scene)
{
    auto it = _s_scenes.find(scene);

    if (scene == _s_active_scene)
        _s_active_scene = nullptr;

    DeleteScene(_s_active_scene);
    if (it != _s_scenes.end())
    {
        _s_scenes.erase(it);
    }
}

void SceneSystem::LoadScene(Scene* scene)
{
    if (!IsSceneRegistered(scene))
    {
        RegisterScene(scene);
    }
    _s_active_scene = scene;
}

void SceneSystem::DeleteScene(Scene* scene)
{
   for(Entity* entity : scene->GetEntities())
   {
       scene->DeregisterEntity(entity);
   }
}

bool SceneSystem::IsSceneRegistered(Scene* scene)
{
    return _s_scenes.find(scene) != _s_scenes.end();
}

const std::unordered_set<Scene*>& SceneSystem::GetScenes()
{
    return _s_scenes;
}
