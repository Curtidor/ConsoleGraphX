#include <windows.h>
#include <utility>
#include "../ConsoleGraphX/application.h"
#include "../ConsoleGraphX/palette.h"
#include "../ConsoleGraphX/screen.h"
#include "../ConsoleGraphX/IGameModule.h"
#include "../ConsoleGraphX/window_manager.h"
#include "../ConsoleGraphX/profiler.h"
#include "../WinCore/WinCore.h"

using namespace ConsoleGraphX;



int main()
{
    SceneSystem sceneSystem;
    Application mainApplication = Application();

    ConsoleGraphX_Internal::CGXProfiler::Initialize();
    ConsoleGraphX_Internal::LoggerManager::Initialize();
    WindowManager::Initialize();

    auto [gameModule, moduleHandle] = WinCore::LoadModule<IGameModule>("Sandbox.dll", "CreateGameModule");
    if (!gameModule)
    {
        ConsoleGraphX_Internal::LoggerManager::Instance().LogMessage("Application", "Failed to load game module.");
        return -1;
    }

    // Initialize and register scenes
    gameModule->RegisterScenes(sceneSystem);
    sceneSystem.LoadScene("Main Scene");

    Scene* s = sceneSystem.GetActiveScene();
    ConsoleGraphX_Internal::ResourceManager::SetActiveManager(&s->_m_resourceManager);

    mainApplication.WarmUp(sceneSystem);

    Palette& defaultPalette = Palette::DefaultPalette();
    ConsoleGraphX_Internal::Screen::SetPalletColors_A(defaultPalette);

    mainApplication.Run(sceneSystem);

    delete gameModule;
    FreeLibrary(moduleHandle);

    return 0;
}
