#include <windows.h>
#include <thread>
#include "../ConsoleGraphX/application.h"
#include "../ConsoleGraphX/palette.h"
#include "../ConsoleGraphX/screen.h"
#include "../ConsoleGraphX/window.h"
#include "../ConsoleGraphX/window_manager.h"
#include "../ConsoleGraphX/window_layout.h"
#include "../ConsoleGraphX/logger_manager.h"
#include "../ConsoleGraphX/IGameModule.h"
#include "../ConsoleGraphX/profiler.h"
#include "../WinCore/WinCore.h"

using namespace ConsoleGraphX;

int main()
{
    short screenWidth = 300;
    short screenHeight = 120;
    short fontWidth = 6;
    short fontHeight = 3;
    
    Application mainApplication = Application(screenWidth, screenHeight, fontWidth, fontHeight);
    SceneSystem sceneSystem;

    ConsoleGraphX_Internal::CGXProfiler::Initialize();

    // Initialize the application
    mainApplication.Initialize();

    Palette& defaultPalette = Palette::DefaultPalette();
    ConsoleGraphX_Internal::Screen::SetPalletColors_A(defaultPalette);

    auto [gameModule, moduleHandle] = LoadModule<IGameModule>("Sandbox.dll", "CreateGameModule");
    if (!gameModule)
    {
        std::cerr << "Failed to load game module." << std::endl;
        return -1;
    }

    gameModule->Initialize();
    gameModule->RegisterScenes(sceneSystem);
    gameModule->LoadInitialScene(sceneSystem);


    Scene* activeScene = sceneSystem.GetActiveScene();
    ConsoleGraphX_Internal::ResourceManager::SetActiveManager(&activeScene->_m_resourceManager);
    
    // Script warm-up
    mainApplication.WarmUp();


    // Create Logger and RTIP windows
    CrossProcessWindow* loggerWindow = WindowManager::Instance().CreateCGXWindow(86, 14, 16, 16, "Logger");
    ConsoleGraphX_Internal::LoggerManager::Instance().AttachWindow(loggerWindow);

    CrossProcessWindow* RTIPWindow = WindowManager::Instance().CreateCGXWindow(60, 38, 16, 16, "RTIP");
    ConsoleGraphX_Internal::CGXProfiler::Instance().AttachWindow(RTIPWindow);

    WindowPositionData wp = WindowManager::Instance().GetWindow("Main")->GetWindowPosition();
    WindowPositionData wpL = WindowManager::Instance().GetWindow("Logger")->GetWindowPosition();


    WindowPositioningRule wpr1 = { WindowManager::Instance().GetWindow("Main"), Anchor::None, Alignment::Below, {0, -5} };
    WindowPositioningRule wpr2 = { WindowManager::Instance().GetWindow("Main"), Anchor::None, Alignment::RightOf, {-8, 0} };
    
    WindowLayout windowLayout;
    windowLayout.AddWindow(loggerWindow, wpr1);
    windowLayout.AddWindow(RTIPWindow, wpr2);

    bool close = false;
    auto closeCallback = [&close]()
        {
            close = true;
        };

    mainApplication.OnClose.AddListener(closeCallback);

    std::thread windowPositioner([&windowLayout, &close]() 
        {
            while (!close)
            {
               windowLayout.ApplyLayout();
            }
        });

    // Run the application
    mainApplication.Run(sceneSystem);

    delete gameModule; 
    FreeLibrary(moduleHandle);

    return 0;
}
