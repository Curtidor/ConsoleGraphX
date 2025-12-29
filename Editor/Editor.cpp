#include <windows.h>
#include <thread>
#include <iostream>
#include <atomic>

#include "./Engine/Core/Window/window.h"
#include "./Engine/Core/Window/window_manager.h"
#include "./Engine/Core/Application/application.h"
#include "./Engine/Core/Application/IGameModule.h"
#include "./Engine/Core/Profiler/profiler.h"
#include "./Engine/Core/Logger/logger_manager.h"
#include "./Engine/Systems/input_system.h"
#include "./Engine/Graphics/palette.h"
#include "./Engine/Graphics/ScreenGraphics/screen.h"
#include "./Engine/Core/Window/Layout/window_layout.h"
#include "./Engine/Systems/scene_system.h"

#include "../WinCore/WinCore.h"
#include "../WinCore/WindowStyleFlags.h"

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
    LoggerManager::Initialize();
    WindowManager::Initialize();
#endif

    InputSystem::Initialize();

    auto [gameModule, handle] = WinCore::LoadModule<IGameModule>("Sandbox.dll", "CreateGameModule");
    moduleHandle = handle;

    if (!gameModule)
    {
        LoggerManager::Instance().LogMessage(
            "Application",
            "Failed to load game module.",
            LoggerManager::LogLevel::CGX_ERROR
        );
        return nullptr;
    }

    return gameModule;
}

/**
*@brief Configures the application windows and layouts.
*
* @param mainApplication Reference to the Application instance.
* @param zOrders Reference to a vector for window Z-ordering.
* @param outerLayout Reference to the outer WindowLayout instance.
* @param innerLayout Reference to the inner WindowLayout instance.
*/
static void ConfigureWindows(
    Application& mainApplication,
    std::vector<WindowZOrder>& zOrders,
    const std::shared_ptr<WindowLayout>& outerLayout,
    const std::shared_ptr<WindowLayout>& innerLayout)
{
#if (MIN_BUILD == 1)
    return; // no window code in min builds
#endif

    auto editorWindow = WindowManager::Instance().CreateCGXWindow<CrossProcessWindow>(250, 75, 8, 8, "Editor");
    (void)editorWindow->OnWindowDestroyed.AddListener(&mainApplication, &Application::OnConsoleClose);

    auto mainWindow = std::static_pointer_cast<Window>(WindowManager::Instance().GetSharedWindow("Main"));

    auto loggerWindow = WindowManager::Instance().CreateCGXWindow<CrossProcessWindow>(60, 10, 12, 12, "Logger");
    LoggerManager::Instance().AttachWindow(loggerWindow.get());
    LoggerManager::Instance().StartLoggerThread(mainApplication.m_engine.m_threadManager);

    auto rtipWindow = WindowManager::Instance().CreateCGXWindow<CrossProcessWindow>(60, 30, 12, 12, "RTIP");
    CGXProfiler::Instance().AttachWindow(rtipWindow.get());

    zOrders = {
        { mainWindow,   0 },
        { editorWindow, 3 },
        { loggerWindow, 2 },
        { rtipWindow,   1 }
    };

    WindowPositioningRule loggerRule = { mainWindow,   Anchor::None,    Alignment::Below,   {0, 0}, ZOrder::Below };
    WindowPositioningRule rtipRule = { mainWindow,   Anchor::None,    Alignment::RightOf, {0, 0}, ZOrder::Below };
    WindowPositioningRule mainRule = { editorWindow, Anchor::TopLeft, Alignment::None,    {8, 31}, ZOrder::Above };

    outerLayout->AddWindow(mainWindow, mainRule);
    innerLayout->AddWindow(loggerWindow, loggerRule);
    innerLayout->AddWindow(rtipWindow, rtipRule);
}

/**
 * @brief Manages the main application loop and window layouts.
 *
 * NOTE:
 * - We realign when editor drag ends (1 poll interval of stability).
 * - We re-apply z-order only when needed:
 *   - on editor focus gained (it jumps top-most during interaction)
 *   - on drag end (same reason)
 *
 * No constant z-order spam.
 */
static void RunApplication(
    Application& mainApplication,
    SceneSystem& sceneSystem,
    std::vector<WindowZOrder>& zOrders,
    const std::shared_ptr<WindowLayout>& outerLayout,
    const std::shared_ptr<WindowLayout>& innerLayout)
{
#if (MIN_BUILD == 0)
    auto& threadManager = mainApplication.m_engine.m_threadManager;

    std::weak_ptr<WindowLayout> weakInner = innerLayout;
    std::weak_ptr<WindowLayout> weakOuter = outerLayout;

    auto editorWindow = std::static_pointer_cast<CrossProcessWindow>(
        WindowManager::Instance().GetSharedWindow("Editor"));

    HWND editorHwnd = nullptr;
    if (editorWindow)
    {
        // Cross-process: HWND may not be ready instantly.
        for (int i = 0; i < 2000; ++i)
        {
            editorHwnd = editorWindow->GetHWND();
            if (editorHwnd && IsWindow(editorHwnd))
                break;
            Sleep(1);
        }
    }

    // Requests consumed ONLY by the WindowPositioner thread.
    std::atomic<bool> requestReorder{ false };
    std::atomic<bool> requestRelayout{ false };

    // When editor gets focus, it can jump front. Request a reorder (don’t do it inline / other thread).
    if (editorWindow)
    {
        (void)editorWindow->OnWindowFocusGained.AddListenerLambda(std::function<void()>(
            [&]()
            {
                requestReorder.store(true, std::memory_order_release);
            }));
    }

    // Listener: relayout + reorder when the editor drag ends.
    CGXEventArgs<int, int, int, int> OnEditorMoved;
    (void)OnEditorMoved.AddListenerLambda(std::function<void(int, int, int, int)>(
        [&](int /*x*/, int /*y*/, int /*w*/, int /*h*/)
        {
            requestRelayout.store(true, std::memory_order_release);
            requestReorder.store(true, std::memory_order_release);
        }));

    // Initial alignment
    if (auto o = weakOuter.lock()) o->ApplyLayout();
    if (auto i = weakInner.lock()) i->ApplyLayout();
    AdjustZOrder(zOrders);

    threadManager.StartThread("WindowPositioner", [&](std::atomic<bool>& shouldQuit)
        {
            WinCore::WindowRectPx lastRect{};
            bool hasLast = false;
            bool editorIsDragging = false;

            while (!shouldQuit.load(std::memory_order_acquire))
            {
                if (!editorHwnd || !IsWindow(editorHwnd))
                {
                    Sleep(200);
                    continue;
                }

                WinCore::WindowRectPx r{};
                if (!WinCore::GetWindowOuterRectPx(editorHwnd, r))
                {
                    Sleep(200);
                    continue;
                }

                const bool changed =
                    (!hasLast) ||
                    r.left != lastRect.left || r.top != lastRect.top ||
                    r.right != lastRect.right || r.bottom != lastRect.bottom;

                if (changed)
                {
                    hasLast = true;
                    lastRect = r;
                    editorIsDragging = true;
                }
                else if (editorIsDragging)
                {
                    // Drag ended (stable for one poll interval)
                    editorIsDragging = false;

                    int x = r.left;
                    int y = r.top;
                    int w = r.Width();
                    int h = r.Height();

                    OnEditorMoved.InvokeNF(x, y, w, h);

                    LoggerManager::Instance().LogMessage(
                        "EditorMoveWatcher",
                        "Editor moved -> relayout + reorder requested.",
                        LoggerManager::LogLevel::CGX_INFO);
                }

                // ---- consume requests (ONLY HERE touches zOrders / layouts) ----
                if (requestReorder.exchange(false, std::memory_order_acq_rel))
                {
                    AdjustZOrder(zOrders);
                }

                if (requestRelayout.exchange(false, std::memory_order_acq_rel))
                {
                    if (auto o = weakOuter.lock()) o->ApplyLayout();
                    if (auto i = weakInner.lock()) i->ApplyLayout();
                }

                Sleep(200);
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

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    {
        Application mainApplication;

#if MIN_BUILD == 1
        g_app = &mainApplication;
#endif

        auto& sceneSystem = *static_cast<SceneSystem*>(
            mainApplication.m_engine.GetSystemManager().GetSystem<SceneSystem>());

        gameModule = InitializeApplication(mainApplication, moduleHandle);
        if (!gameModule)
            return -1;

        gameModule->RegisterScenes(sceneSystem);
        sceneSystem.LoadScene("Main Scene");

        Scene* activeScene = sceneSystem.GetActiveScene();
        ResourceManager::SetActiveManager(&activeScene->_m_resourceManager);

        mainApplication.WarmUp(sceneSystem);
        Screen::SetPalletColors_A(Palette::DefaultPalette());

        std::vector<WindowZOrder> zOrders;
        auto outerLayout = std::make_shared<WindowLayout>();
        auto innerLayout = std::make_shared<WindowLayout>();

        ConfigureWindows(mainApplication, zOrders, outerLayout, innerLayout);
        WindowManager::Instance().MonitorWindowCloses(mainApplication.m_engine.m_threadManager);

        RunApplication(mainApplication, sceneSystem, zOrders, outerLayout, innerLayout);
    }

    delete gameModule;
    FreeLibrary(moduleHandle);
    return 0;
}
