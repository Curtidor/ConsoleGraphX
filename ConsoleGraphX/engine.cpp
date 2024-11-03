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
#include "window_manager.h"
//graphics
#include "renderer.h"
#include "screen.h"
//others
#include "logger.h"


namespace ConsoleGraphX
{
    Engine::Engine(short screenWidth, short screenHeight, short pixelWidth, short pixelHeight)
        : _m_logger(ConsoleGraphX_Internal::Logger("Engine")), _m_systemManager(ConsoleGraphX_Internal::SystemManager())
    {
        _m_systemManager.RegisterSystem<ScriptSystem>();
        _m_systemManager.RegisterSystem<PlayerControllerSystem>();

        // my bad
        _m_window = new EmbeddedWindow(
            screenWidth, screenHeight, "Main", pixelWidth, pixelHeight
        );

        ConsoleGraphX_Internal::Screen::SetActiveScreen_A(_m_window);
    }

    void Engine::Initialize()
    {
        ConsoleGraphX_Internal::LoggerManager::Initialize();

        std::string& windowName = _m_window->GetWindowNameR();
        
        // i'd like to apologize for what's below
        std::unique_ptr<EmbeddedWindow> uniqueEWindow = std::unique_ptr<EmbeddedWindow>(_m_window);
        WindowManager::Instance().RegisterWindow(std::move(uniqueEWindow));

        _m_window = static_cast<EmbeddedWindow*>(WindowManager::Instance().GetWindow(windowName));
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
        _m_window->FillCanvas(CHAR_INFO{ _m_window->s_pixel, 0 });
        ConsoleGraphX_Internal::Renderer::DrawSprites(alpha);
        _m_window->DrawScreen();

    }

};
