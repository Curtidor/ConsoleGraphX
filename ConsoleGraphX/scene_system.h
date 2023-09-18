#pragma once
#include <unordered_set>
#include "system.h"
#include "scene.h"

class SceneSystem
{
private:
    static Scene* _s_active_scene;
    static std::unordered_set<Scene*> _s_scenes;

public:
    static void RegisterScene(Scene* scene);
    static void DeregisterScene(Scene* scene);
    static void LoadScene(Scene* scene);
    static void DeleteScene(Scene* scene);
    static bool IsSceneRegistered(Scene* scene);
    static const std::unordered_set<Scene*>& GetScenes(); // Updated return type
};
