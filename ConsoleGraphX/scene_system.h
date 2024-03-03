#pragma once
#include <unordered_map>
#include "scene.h"
#include <string>

class SceneSystem
{
private:
    static Scene* _s_activeScene;
    static std::unordered_map<std::string, Scene*> _s_scenes;

public:
    static void RegisterScene(Scene* scene);
    static void DeregisterScene(const std::string& name);
    static void LoadScene(const std::string& name);
    static void DeleteScene(const std::string& name);
    
    static bool IsSceneRegistered(const std::string& name);
    
    static Scene* GetActiveScene();
    static const std::unordered_map<std::string, Scene*>& GetScenes(); // Updated return type
};
