#include <windows.h>
#include <utility>
#include "./Engine/Core/Application/application.h"
#include "./Engine/Core/Application/IGameModule.h"
#include "./Engine/Core/Window/window_manager.h"
#include "./Engine/Core/Profiler/profiler.h"
#include "./Engine/Systems/input_system.h"
#include "./Engine/Graphics/palette.h"
#include "./Engine/Graphics/ScreenGraphics/screen.h""
#include "./WinCore.h"

using namespace ConsoleGraphX;
using namespace ConsoleGraphX_Internal;


int main()
{
    ConsoleGraphX::SceneSystem sceneSystem;
    Application mainApplication = Application();

    CGXProfiler::Initialize();
    LoggerManager::Initialize();
    WindowManager::Initialize();
    InputSystem::Initialize();

    auto [gameModule, moduleHandle] = WinCore::LoadModule<IGameModule>("Sandbox.dll", "CreateGameModule");
    if (!gameModule)
    {
        LoggerManager::Instance().LogMessage("Application", "Failed to load game module.");
        return -1;
    }

    // Initialize and register scenes
    gameModule->RegisterScenes(sceneSystem);
    sceneSystem.LoadScene("Main Scene");

    Scene* s = sceneSystem.GetActiveScene();
    ResourceManager::SetActiveManager(&s->_m_resourceManager);

    mainApplication.WarmUp(sceneSystem);

    Palette& defaultPalette = Palette::DefaultPalette();
    Screen::SetPalletColors_A(defaultPalette);

    mainApplication.Run(sceneSystem);

    delete gameModule;
    FreeLibrary(moduleHandle);

    return 0;
}
