#include "PCH_CGX.h"
#include "WinCore.h"
#include "Engine\Core\Application\engine.h"
// managers
#include "Engine\Core\Application\system_manager.h"
#include "Engine\Core\Logger\logger_manager.h"
#include "Engine\Core\Window\window_manager.h"
#include "Engine\Core\Concurrency\thread_manager.h"
#include "Engine\Resources\resource_manager.h"
// systems
#include "Engine\Systems\input_system.h"
#include "Engine\Systems\player_controller_system.h"
#include "Engine\Systems\script_system.h"
#include "Engine\Systems\scene_system.h"
// graphics
#include "Engine\Graphics\renderer.h"
#include "Engine\Graphics\ScreenGraphics\screen.h"
// others
#include "Engine\Core\Logger\logger.h"
#include "Engine\Core\Profiler\profiler.h"

namespace ConsoleGraphX
{
    Engine::Engine()
        : _m_logger(ConsoleGraphX_Internal::Logger("Engine")),
        _m_systemManager(ConsoleGraphX_Internal::SystemManager()),
        m_threadManager(ConsoleGraphX_Internal::ThreadManager())
    {
        _m_systemManager.RegisterSystem<ScriptSystem>();
        _m_systemManager.RegisterSystem<PlayerControllerSystem>();
        _m_systemManager.RegisterSystem<SceneSystem>();
    #if MIN_BUILD == 1
        _m_window = nullptr;
    #endif 

    }

    void Engine::AttachWindow(std::shared_ptr<Window> window)
    {
    #if MIN_BUILD == 0
        CGX_VERIFY(window);
        _m_window = std::move(window);
    #endif
    }

    void Engine::WarmUp(SceneSystem& sceneSystem)
    {
    #if MIN_BUILD == 1
        if (!_m_window)
        {
            _m_window = new ConsoleGraphX_Internal::Screen(300, 120, 3, 3);
            ConsoleGraphX_Internal::Screen::SetActiveScreen_A(_m_window);
        }
    #else
        if (!_m_window)
        {
            _m_window = WindowManager::Instance().CreateCGXWindow<Window>(300, 120, 3, 3, "Main");
            ConsoleGraphX_Internal::Screen::SetActiveScreen_A(_m_window.get());
        }
    #endif

        ScriptSystem::ScriptWarmUp(sceneSystem);
    }

    void Engine::UpdateSystems(float deltaTime, SceneSystem& sceneSystem)
    {
        InputSystem::Instance().GetPressedKeys();
        _m_systemManager.Update(deltaTime, sceneSystem);
    }

    void Engine::Render(SceneSystem& sceneSystem, float alpha)
    {
        _m_window->FillCanvas(CHAR_INFO{ _m_window->s_pixel, 6 });
        ConsoleGraphX_Internal::Renderer::DrawSprites(*_m_window, sceneSystem, alpha);
        _m_window->DrawScreen();

    #if MIN_BUILD == 0
        ConsoleGraphX_Internal::CGXProfiler::Instance().DisplayMetrics();
    #endif
    }

    ConsoleGraphX_Internal::SystemManager& Engine::GetSystemManager()
    {
        return _m_systemManager;
    }

    void Engine::Shutdown()
    {
        _m_systemManager.ShutDown();
    }

}
