#include <windows.h>
#include <thread>
#include <iostream>
#include "./Engine/Core/Window/window.h"
#include "./Engine/Core/Window/window_styles.h"
#include "./Engine/Core/Window/window_manager.h"
#include "./Engine/Core/Application/application.h"
#include "./Engine/Core/Application/IGameModule.h"
#include "./Engine/Core/Profiler/profiler.h"
#include "./Engine/Core/Logger/logger_manager.h"
#include "./Engine/Systems/input_system.h"
#include "./Engine/Graphics/palette.h"
#include "./Engine/Graphics/ScreenGraphics/screen.h"
#include "./Engine/Layout/window_layout.h"
#include "./Engine/Systems/scene_system.h"
#include "../WinCore/WinCore.h"

using namespace ConsoleGraphX;
using namespace ConsoleGraphX_Internal;


/**
 * @brief Initializes the application and systems.
 *
 * @return Pointer to the loaded game module, or nullptr on failure.
 */
static IGameModule* InitializeApplication(Application& app, HMODULE& moduleHandle)
{
    #if defined(DEBUG) && (MIN_BUILD == 0)
    CGXProfiler::Initialize();
    LoggerManager::Initialize(app.m_engine.m_threadManager);
    WindowManager::Initialize();
    #endif

    InputSystem::Initialize();

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
*@brief Configures the application windows and layouts.
*
* @param mainApplication Reference to the Application instance.
* @param zOrders Reference to a vector for window Z - ordering.
* @param outerLayout Reference to the outer WindowLayout instance.
* @param innerLayout Reference to the inner WindowLayout instance.
*/
static void ConfigureWindows(
    Application& mainApplication,
    std::vector<WindowZOrder>& zOrders,
    std::shared_ptr<WindowLayout> outerLayout,
    std::shared_ptr<WindowLayout> innerLayout)
{
#if (MIN_BUILD == 1)
    return; // no window code in min builds
#endif

    auto editorWindow = WindowManager::Instance().CreateCGXWindow<CrossProcessWindow>(144, 40, 16, 16, "Editor");
    EventCallBackHandle handle = editorWindow->OnWindowDestroyed.AddListener(&mainApplication, &Application::OnConsoleClose);

    auto mainWindow = std::static_pointer_cast<Window>(WindowManager::Instance().GetSharedWindow("Main"));

    auto loggerWindow = WindowManager::Instance().CreateCGXWindow<CrossProcessWindow>(86, 12, 16, 16, "Logger");
    LoggerManager::Instance().AttachWindow(loggerWindow.get());

    auto RTIPWindow = WindowManager::Instance().CreateCGXWindow<CrossProcessWindow>(58, 20, 16, 16, "RTIP");
    CGXProfiler::Instance().AttachWindow(RTIPWindow.get());

    Sleep(1000); // TEMP workaround for HWND readiness

    zOrders = {
        { mainWindow, 0 },
        { editorWindow, 3 },
        { loggerWindow, 2 },
        { RTIPWindow, 1 }
    };

    for (const auto& window : WindowManager::Instance().GetAllSharedWindows())
        window->SetHWND(window->GetHWND());

    ApplyWindowStyles(WindowStyles::Borderless, mainWindow->GetHWND());

    WindowPositioningRule loggerRule = { mainWindow.get(), Anchor::None, Alignment::Below, {0, 0}, ZOrder::Below };
    WindowPositioningRule rtipRule = { mainWindow.get(), Anchor::None, Alignment::RightOf, {0, 0}, ZOrder::Below };
    WindowPositioningRule mainRule = { editorWindow.get(), Anchor::TopLeft, Alignment::None, {8, 31}, ZOrder::Above };

    outerLayout->AddWindow(mainWindow, mainRule);
    innerLayout->AddWindow(loggerWindow, loggerRule);
    innerLayout->AddWindow(RTIPWindow, rtipRule);
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
static void RunApplication(
    Application& mainApplication,
    SceneSystem& sceneSystem,
    std::vector<WindowZOrder>& zOrders,
    std::shared_ptr<WindowLayout> outerLayout,
    std::shared_ptr<WindowLayout> innerLayout)
{
    #if (MIN_BUILD == 0)
    auto& threadManager = mainApplication.m_engine.m_threadManager;

    std::weak_ptr<WindowLayout> weakInner = innerLayout;
    std::weak_ptr<WindowLayout> weakOuter = outerLayout;


  threadManager.StartThread("WindowPositioner", [&](std::atomic<bool>& shouldQuit) {
        while (!shouldQuit.load(std::memory_order_acquire)) {
            AdjustZOrder(zOrders);
            Sleep(200);

            if (auto inner = weakInner.lock()) 
                inner->ApplyLayout();
            if (auto outer = weakOuter.lock()) 
                outer->ApplyLayout();
        }
        });
    LoggerManager::Instance().LogMessage("Application", "Starting Application...");
    #endif
    mainApplication.Run(sceneSystem);
}

// ========================= WARNING FOR FUTURE ME ==========================
// DO NOT move gameModule deletion or FreeLibrary(moduleHandle) above this block.
//
// Reason:
// - The Application (and everything it owns: SceneSystem, ResourceManager, Entities, etc.)
//   must be fully destroyed BEFORE unloading the game module DLL.
// - Destroying the game module first will cause Entity::Kill() or 
//   DestroyEntityResources() to access freed or unmapped memory → crash.
//
// TL;DR:
// Keep all engine + scene logic scoped inside this block.
// Only unload the DLL AFTER the Application and its subsystems are dead.
//
// (THIS TOOK DAYS TO FIND)
// =====================================================================================
int main()
{
    HMODULE moduleHandle = nullptr;
    IGameModule* gameModule = nullptr;

    {
        Application mainApplication;

    #if MIN_BUILD == 1
        g_app = &mainApplication;
    #endif

        // initialize engine module
        auto& sceneSystem = *static_cast<SceneSystem*>(
            mainApplication.m_engine.GetSystemManager().GetSystem<SceneSystem>());

        gameModule = InitializeApplication(mainApplication, moduleHandle);
        if (!gameModule)
            return -1;

        // setup scene
        gameModule->RegisterScenes(sceneSystem);
        sceneSystem.LoadScene("Main Scene");

        Scene* activeScene = sceneSystem.GetActiveScene();
        ResourceManager::SetActiveManager(&activeScene->_m_resourceManager);

        //warm up and setup 
        mainApplication.WarmUp(sceneSystem);

        // set default palette colors
        Screen::SetPalletColors_A(Palette::DefaultPalette());

        // window layout setup 
        std::vector<WindowZOrder> zOrders;
        auto outerLayout = std::make_shared<WindowLayout>();
        auto innerLayout = std::make_shared<WindowLayout>();

        ConfigureWindows(mainApplication, zOrders, outerLayout, innerLayout);
        WindowManager::Instance().MonitorWindowCloses(mainApplication.m_engine.m_threadManager);

        RunApplication(mainApplication, sceneSystem, zOrders, outerLayout, innerLayout);
    }

    // cleanup
    delete gameModule;
    FreeLibrary(moduleHandle);

    return 0;
}
