#include <windows.h>
#include <utility>
#include "../ConsoleGraphX/application.h"
#include "../ConsoleGraphX/palette.h"
#include "../ConsoleGraphX/screen.h"
#include "../ConsoleGraphX/IGameModule.h"
#include "../ConsoleGraphX/profiler.h"
#include "../WinCore/WinCore.h"

using namespace ConsoleGraphX;



int main()
{
    SceneSystem sceneSystem;
    Application mainApplication = Application(300, 120, 6, 3);

    // Initialize the application
    mainApplication.Initialize();

    ConsoleGraphX_Internal::CGXProfiler::Initialize();

    Palette& defaultPalette = Palette::DefaultPalette();
    ConsoleGraphX_Internal::Screen::SetPalletColors_A(defaultPalette);

    auto [gameModule, moduleHandle] = LoadModule<IGameModule>("Sandbox.dll", "CreateGameModule");
    if (!gameModule)
    {
        std::cerr << "Failed to load game module." << std::endl;
        return -1;
    }

    // Initialize and register scenes
    gameModule->Initialize();
    gameModule->RegisterScenes(sceneSystem);
    gameModule->LoadInitialScene(sceneSystem);

    Scene* s = sceneSystem.GetActiveScene();
    ConsoleGraphX_Internal::ResourceManager::SetActiveManager(&s->_m_resourceManager);

    mainApplication.WarmUp();

    mainApplication.Run(sceneSystem);

    delete gameModule;
    FreeLibrary(moduleHandle);

    return 0;
}
