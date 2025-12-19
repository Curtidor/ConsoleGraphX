#include <memory>
#include "../ConsoleGraphX/Engine/Core/Application/IGameModule.h"
#include "main_scene.h"
#include "second_scene.h"

using namespace ConsoleGraphX;

// Destroy function - DLL-side
extern "C" __declspec(dllexport)
void DestroyScene(Scene* scene)
{
    delete scene;
}

// Scene Factory Helper
template<typename T>
std::unique_ptr<Scene, void(*)(Scene*)> MakeScene()
{
    return std::unique_ptr<Scene, void(*)(Scene*)>(
        new T(),
        [](Scene* ptr) { DestroyScene(ptr); }
    );
}


// Game Module Implementation
class SandboxGameModule : public IGameModule
{
public:
    void RegisterScenes(ConsoleGraphX::SceneSystem& system) override
    {
        system.RegisterScene(MakeScene<MainScene>());
        system.RegisterScene(MakeScene<SecondScene>());
    }
};

// Entry Point for Engine
extern "C" __declspec(dllexport)
IGameModule * CreateGameModule()
{
    return new SandboxGameModule();
}
