#include "PCH_CGX.h"
#include "WinCore.h"
#include "engine.h"
// managers
#include "system_manager.h"
#include "logger_manager.h"
// systems
#include "input_system.h"
#include "player_controller_system.h"
#include "script_system.h"
//graphics
#include "renderer.h"
#include "screen.h"
//others
#include "logger.h"


namespace ConsoleGraphX
{
    Engine::Engine(short screenWidth, short screenHeight, short pixelWidth, short pixelHeight)
        : _m_logger(ConsoleGraphX_Internal::Logger("Engine")), _m_systemManager(ConsoleGraphX_Internal::SystemManager()), 
        _m_screen(ConsoleGraphX_Internal::Screen(screenWidth, screenHeight, pixelWidth, pixelHeight))
    {
        _m_systemManager.RegisterSystem<ScriptSystem>();
        _m_systemManager.RegisterSystem<PlayerControllerSystem>();


        ConsoleGraphX_Internal::Screen::SetActiveScreen_A(&_m_screen);
    }

    void Engine::Initialize()
    {
        ConsoleGraphX_Internal::LoggerManager::Initialize();
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
        ConsoleGraphX_Internal::LoggerManager::ShutDown(); // should be the last thing to close
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
        _m_screen.FillCanvas(CHAR_INFO{ _m_screen.s_pixel, 0 });
        ConsoleGraphX_Internal::Renderer::DrawSprites(alpha);
        _m_screen.DrawScreen();

    }

    void Engine::UpdateFPS(int fps)
    {
        SetConsoleNameWC("FPS: " + std::to_string(fps));
    }

};
