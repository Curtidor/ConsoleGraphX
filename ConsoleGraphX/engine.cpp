#include "CGXPCH.h"
#include "engine.h"
#include "logger.h"
#include "system_manager.h"
#include "input_system.h"
#include "player_controller_system.h"
#include "renderer.h"
#include "screen.h"
#include "script_system.h"
#include "logger.h"
#include "debugger_manager.h"
#include "script_system.h"
#include "window_manager.h"

namespace ConsoleGraphX
{
    Engine::Engine(short screen_width, short screen_height, short pixel_width, short pixel_height)
        : _m_logger(ConsoleGraphX_Internal::Logger("Engine")), _m_systemManager(ConsoleGraphX_Internal::SystemManager()), 
        _m_screen(ConsoleGraphX_Internal::Screen(screen_width, screen_height, pixel_width, pixel_height))
    {
        ConsoleGraphX_Internal::Screen::SetActiveScreen_A(&_m_screen);

        _m_systemManager.RegisterSystem<ScriptSystem>();
        _m_systemManager.RegisterSystem<PlayerControllerSystem>();

    }

    void Engine::Initialize()
    {
        ConsoleGraphX_Internal::DebuggerManager::Initialize();
        WindowManager::Initialize();
    }

    void Engine::WarmUp()
    {
        ScriptSystem::ScriptWarmUp();
    }

    void Engine::Start()
    {
        _m_isRunning = true;
    }

    void Engine::Shutdown()
    {
        _m_isRunning = false;
        WindowManager::ShutDown();
        ConsoleGraphX_Internal::DebuggerManager::ShutDown(); // should be the last thing to close
    }

    bool Engine::IsRunning() const
    {
        return _m_isRunning;
    }

    void Engine::UpdateSystems(float deltaTime)
    {
        InputSystem::GetPressedKeys();
        _m_systemManager.Update(deltaTime);
    }

    void Engine::Render(float alpha)
    {
        _m_screen.FillScreen(CHAR_INFO{ _m_screen.s_pixel, 0 });
        ConsoleGraphX_Internal::Renderer::DrawSprites(alpha);
        _m_screen.DrawScreen();
    }

    void Engine::UpdateFPS(int fps)
    {
        _m_screen.SetConsoleName("FPS: " + std::to_string(fps));
    }

};
