#include "PCH_CGX.h"
#include "WinCore.h"
#include "engine.h"
// managers
#include "system_manager.h"
#include "logger_manager.h"
#include "resource_manager.h"
// systems
#include "input_system.h"
#include "player_controller_system.h"
#include "script_system.h"
#include "window_manager.h"
//graphics
#include "renderer.h"
#include "screen.h"
//others
#include "logger.h"
#include "profiler.h"


namespace ConsoleGraphX
{
    Engine::Engine()
        : _m_logger(ConsoleGraphX_Internal::Logger("Engine")), _m_systemManager(ConsoleGraphX_Internal::SystemManager()), _m_window(nullptr)
    {
        _m_systemManager.RegisterSystem<ScriptSystem>();
        _m_systemManager.RegisterSystem<PlayerControllerSystem>();
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
        InputSystem::GetPressedKeys();
        _m_systemManager.Update(deltaTime, sceneSystem);
    }

    void Engine::Render(SceneSystem& sceneSystem, float alpha)
    {
        _m_window->FillCanvas(CHAR_INFO{ _m_window->s_pixel, 6 });
        ConsoleGraphX_Internal::Renderer::DrawSprites(*_m_window, sceneSystem, alpha);
        _m_window->DrawScreen();

        ConsoleGraphX_Internal::CGXProfiler::Instance().DisplayMetrics();
    }

    void Engine::Shutdown()
    {
        // TODO: add engine shut down
    }

};
