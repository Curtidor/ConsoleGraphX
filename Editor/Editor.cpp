#include <windows.h>
#include <thread>
#include "../ConsoleGraphX/application.h"
#include "../ConsoleGraphX/palette.h"
#include "../ConsoleGraphX/screen.h"
#include "../ConsoleGraphX/window.h"
#include "../ConsoleGraphX/window_manager.h"
#include "../ConsoleGraphX/window_layout.h"
#include "../ConsoleGraphX/window_styles.h"
#include "../ConsoleGraphX/logger_manager.h"
#include "../ConsoleGraphX/IGameModule.h"
#include "../ConsoleGraphX/profiler.h"
#include "../WinCore/WinCore.h"

using namespace ConsoleGraphX;

// NOTE TO SELF: i feel like the engine shouldnt live in the application rather it should be created in the main function


int main()
{   
    Application mainApplication = Application();
    SceneSystem sceneSystem;

    ConsoleGraphX_Internal::CGXProfiler::Initialize();
    mainApplication.Initialize();

    auto [gameModule, moduleHandle] = WinCore::LoadModule<IGameModule>("Sandbox.dll", "CreateGameModule");
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
    
    // Script warm-up and screen init if needed
    mainApplication.WarmUp();

    Palette& defaultPalette = Palette::DefaultPalette();
    ConsoleGraphX_Internal::Screen::SetPalletColors_A(defaultPalette);


    // Create Logger and RTIP windows
    CrossProcessWindow* parent = WindowManager::Instance().CreateCGXWindow(144, 40, 16, 16, "Editor");

    CrossProcessWindow* loggerWindow = WindowManager::Instance().CreateCGXWindow(86, 14, 16, 16, "Logger");
    ConsoleGraphX_Internal::LoggerManager::Instance().AttachWindow(loggerWindow);

    CrossProcessWindow* RTIPWindow = WindowManager::Instance().CreateCGXWindow(58, 38, 16, 16, "RTIP");
    ConsoleGraphX_Internal::CGXProfiler::Instance().AttachWindow(RTIPWindow);

    Window* mainWindow = static_cast<Window*>(WindowManager::Instance().GetWindow("Main"));
    CrossProcessWindow* editor = static_cast<CrossProcessWindow*>(WindowManager::Instance().GetWindow("Logger"));

    WindowPositioningRule loggerWindowPRule = { mainWindow, Anchor::None, Alignment::Below, {0, 0}, ZOrder::Below };
    WindowPositioningRule RTIPWindowPRule = { mainWindow, Anchor::None, Alignment::RightOf, {0, 0} , ZOrder::Below };
    WindowPositioningRule mainWindowPRule = { WindowManager::Instance().GetWindow("Editor"), Anchor::TopLeft, Alignment::None, {8, 31}, ZOrder::Above};

    // why 7? idk?, however the offset of 31 is because the title of the console is 31 pixels
    // so to set the main window below the title and aligned with the left sid we use {8,31}

    std::vector<WindowZOrder> orders = 
    {
     { mainWindow->GetHWND(), 0 },  
     { editor->GetHWND(), 1 },
     { loggerWindow->GetHWND(), 2 },
     { RTIPWindow->GetHWND(), 3 }
    };

        
   ApplyWindowStyles(WindowStyles::Borderless, WindowManager::Instance().GetWindow("Main")->GetHWND());

    WindowLayout windowLayoutOuter;
    windowLayoutOuter.AddWindow(WindowManager::Instance().GetWindow("Main"), mainWindowPRule);

    WindowLayout windowLayoutInner;
    windowLayoutInner.AddWindow(loggerWindow, loggerWindowPRule);
    windowLayoutInner.AddWindow(RTIPWindow, RTIPWindowPRule);

    bool close = false;
    auto closeCallback = [&close]()
        {
            close = true;
        };

    mainApplication.OnClose.AddListener(closeCallback);

    std::thread windowPositioner([&windowLayoutInner, &windowLayoutOuter, &close, &orders]()
        {
            while (!close)
            {
               AdjustZOrder(orders);

               Sleep(200);

               windowLayoutInner.ApplyLayout();
               windowLayoutOuter.ApplyLayout();
            }
        });

    // Run the application
    ConsoleGraphX_Internal::LoggerManager::Instance().LogMessage("Application", "Starting Application...");
    mainApplication.Run(sceneSystem);

    windowPositioner.join();


    delete gameModule; 
    FreeLibrary(moduleHandle);

    return 0;
}
