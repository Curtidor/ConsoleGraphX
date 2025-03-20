#include "PCH_CGX.h"
#include "WinCore.h"
#include "Engine\Core\Application\engine.h"
// managers
#include "Engine\Core\Application\system_manager.h"
#include "Engine\Core\Logger\logger_manager.h"
#include "Engine\Core\Window\window_manager.h"
#include "Engine\Resources\resource_manager.h"

// systems
#include "Engine\Systems\input_system.h"
#include "Engine\Systems\player_controller_system.h"
#include "Engine\Systems\script_system.h"
#include "Engine\Systems\scene_system.h"
//graphics
#include "Engine\Graphics\renderer.h"
#include "Engine\Graphics\ScreenGraphics\screen.h"
//others
#include "Engine\Core\Logger\logger.h"
#include "Engine\Core\Profiler\profiler.h"


namespace ConsoleGraphX
{
    Engine::Engine()
        : _m_logger(ConsoleGraphX_Internal::Logger("Engine")), _m_systemManager(ConsoleGraphX_Internal::SystemManager()), _m_window(nullptr)
    {
        _m_systemManager.RegisterSystem<ScriptSystem>();
        _m_systemManager.RegisterSystem<PlayerControllerSystem>();
        _m_systemManager.RegisterSystem<SceneSystem>();
    }

    void Engine::AttachWindow(Window* window)
    {
        CGX_VERIFY(window);

        _m_window = window;
    }

    void Engine::WarmUp(SceneSystem& sceneSystem)
    {
        if (_m_window == nullptr)
        {
            _m_window =  static_cast<Window*>(WindowManager::Instance().CreateCGXWindow<Window>(300, 120, 3, 3, "Main"));

            ConsoleGraphX_Internal::Screen::SetActiveScreen_A(_m_window);
        }

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

        ConsoleGraphX_Internal::CGXProfiler::Instance().DisplayMetrics();
    }

    ConsoleGraphX_Internal::SystemManager& Engine::GetSystemManager()
    {
        return _m_systemManager;
    }


    void Engine::Shutdown()
    {
        // TODO: add engine shut down
    }

};
