/**
 * @file main.cpp
 * @brief Entry point for the ConsoleGraphX application.
 *
 * This file initializes the application, loads the game module, configures windows,
 * and manages the main application loop.
 */

#include <windows.h>
#include <thread>
#include <iostream>
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
using namespace ConsoleGraphX_Internal;

/**
 * @brief Initializes the application and systems.
 *
 * @return Pointer to the loaded game module, or nullptr on failure.
 */
static IGameModule* InitializeApplication(HMODULE& moduleHandle)
{
    CGXProfiler::Initialize();
    LoggerManager::Initialize();
    WindowManager::Initialize();

    auto [gameModule, handle] = WinCore::LoadModule<IGameModule>("Sandbox.dll", "CreateGameModule");
    moduleHandle = handle;

    if (!gameModule) 
    {
        LoggerManager::Instance().LogMessage("Application", "Failed to load game module.", LoggerManager::LogLevel::CGX_ERROR);
        return nullptr;
    }

    return gameModule;
}

/**
 * @brief Configures the application windows and layouts.
 *
 * @param mainApplication Reference to the Application instance.
 * @param zOrders Reference to a vector for window Z-ordering.
 * @param outerLayout Reference to the outer WindowLayout instance.
 * @param innerLayout Reference to the inner WindowLayout instance.
 */
static void ConfigureWindows(Application& mainApplication, std::vector<WindowZOrder>& zOrders, WindowLayout& outerLayout, WindowLayout& innerLayout)
{
    Window* mainWindow = static_cast<Window*>(WindowManager::Instance().GetWindow("Main")); // FWI, the main window is made during the warmup phase of the application

    CrossProcessWindow* loggerWindow = WindowManager::Instance().CreateCGXWindow<CrossProcessWindow>(86, 14, 16, 16, "Logger");
    LoggerManager::Instance().AttachWindow(loggerWindow);

    CrossProcessWindow* editorWindow = WindowManager::Instance().CreateCGXWindow<CrossProcessWindow>(144, 40, 16, 16, "Editor");
    editorWindow->OnWindowDestroyed.AddListener(&mainApplication, &Application::OnConsoleClose);

    CrossProcessWindow* RTIPWindow = WindowManager::Instance().CreateCGXWindow<CrossProcessWindow>(58, 38, 16, 16, "RTIP");
    CGXProfiler::Instance().AttachWindow(RTIPWindow);

    Sleep(1000); //TODO: sleep is here so we can be sure the HWND are going to be available, this is a temp solution 

    zOrders = 
    {
        { mainWindow->GetHWND(), 0 },
        { editorWindow->GetHWND(), 3 },
        { loggerWindow->GetHWND(), 2 },
        { RTIPWindow->GetHWND(), 1 }
    };

    for (AbstractWindow* window : WindowManager::Instance().GetAllWindows())
    {
        window->SetHWND(window->GetHWND()); // we do this so the windows will have a const and predictable HWND, if we don;t set it future calls to GetHWND rely on FindWindow
    }

    ApplyWindowStyles(WindowStyles::Borderless, mainWindow->GetHWND());

    WindowPositioningRule loggerWindowRule = { mainWindow, Anchor::None, Alignment::Below, {0, 0}, ZOrder::Below };
    WindowPositioningRule RTIPWindowRule = { mainWindow, Anchor::None, Alignment::RightOf, {0, 0}, ZOrder::Below };
    WindowPositioningRule mainWindowRule = { editorWindow, Anchor::TopLeft, Alignment::None, {8, 31}, ZOrder::Above };

    outerLayout.AddWindow(mainWindow, mainWindowRule);
    innerLayout.AddWindow(loggerWindow, loggerWindowRule);
    innerLayout.AddWindow(RTIPWindow, RTIPWindowRule);
}

/**
 * @brief Manages the main application loop and window layouts.
 *
 * @param mainApplication Reference to the Application instance.
 * @param sceneSystem Reference to the SceneSystem instance.
 * @param zOrders Reference to a vector for window Z-ordering.
 * @param outerLayout Reference to the outer WindowLayout instance.
 * @param innerLayout Reference to the inner WindowLayout instance.
 * @param isClosing Reference to the closing state flag.
 */
static void RunApplication(Application& mainApplication, SceneSystem& sceneSystem, std::vector<WindowZOrder>& zOrders, WindowLayout& outerLayout, WindowLayout& innerLayout, bool& isClosing)
{
    std::thread windowPositioner([&]()
        {
        while (!isClosing) // bool by ref, :-( mb
        {
            AdjustZOrder(zOrders);

            Sleep(200);

            innerLayout.ApplyLayout();
            outerLayout.ApplyLayout();
        }
        });

    LoggerManager::Instance().LogMessage("Application", "Starting Application...");
    mainApplication.Run(sceneSystem);

    windowPositioner.join();
}

/**
 * @brief Main entry point for the application.
 *
 * Initializes the application, loads the game module, sets up windows,
 * and starts the main application loop.
 *
 * @return Exit status of the application.
 */
int main() 
{
    Application mainApplication;
    SceneSystem sceneSystem;
    HMODULE moduleHandle = nullptr;

    IGameModule* gameModule = InitializeApplication(moduleHandle);
    if (!gameModule) 
    {
        return -1;
    }

    gameModule->RegisterScenes(sceneSystem);
    sceneSystem.LoadScene("Main Scene");

    Scene* activeScene = sceneSystem.GetActiveScene();
    ResourceManager::SetActiveManager(&activeScene->_m_resourceManager);

    mainApplication.WarmUp(sceneSystem);
    Palette& defaultPalette = Palette::DefaultPalette();
    Screen::SetPalletColors_A(defaultPalette);

    std::vector<WindowZOrder> zOrders;
    WindowLayout outerLayout;
    WindowLayout innerLayout;

    ConfigureWindows(mainApplication, zOrders, outerLayout, innerLayout);

    bool isClosing = false;
    auto closeCallback = [&isClosing]() { isClosing = true; };
    mainApplication.OnClose.AddListener(closeCallback);
    
    WindowManager::Instance().MonitorWindowCloses(); // kick off the monitoring thread

    RunApplication(mainApplication, sceneSystem, zOrders, outerLayout, innerLayout, isClosing);

    delete gameModule;
    FreeLibrary(moduleHandle);


`    return 0;
}
