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
    Engine::Engine(short screenWidth, short screenHeight, short pixelWidth, short pixelHeight)
        : _m_logger(ConsoleGraphX_Internal::Logger("Engine")), _m_systemManager(ConsoleGraphX_Internal::SystemManager()), _m_isRunning(false)
    {
        _m_systemManager.RegisterSystem<ScriptSystem>();
        _m_systemManager.RegisterSystem<PlayerControllerSystem>();

        // my bad
        _m_window = new Window(
            screenWidth, screenHeight, pixelWidth, pixelHeight, "Main"
        );

        ConsoleGraphX_Internal::Screen::SetActiveScreen_A(_m_window);
    }

    void Engine::Initialize()
    {
        ConsoleGraphX_Internal::LoggerManager::Initialize();

        std::string& windowName = _m_window->GetWindowNameR();
        
        // i'd like to apologize for what's below
        std::unique_ptr<Window> uniqueWindow = std::unique_ptr<Window>(_m_window); 
        // what's above looks a bit suspicious, but remember wrapping a raw ptr in a smart ptr does not invalidate the raw ptr
        // so the screen will still have a valid pointer

        WindowManager::Instance().RegisterWindow(std::move(uniqueWindow));

        _m_window = static_cast<Window*>(WindowManager::Instance().GetWindow(windowName));

        ConsoleGraphX_Internal::LoggerManager::Instance().LogMessage("Engine", "Engine Initialized.");
    }

    void Engine::WarmUp()
    {
        ScriptSystem::ScriptWarmUp();
        ConsoleGraphX_Internal::LoggerManager::Instance().LogMessage("Engine", "Engine Warmed Up..");
    }

    void Engine::Start()
    {
        _m_isRunning = true;
        ConsoleGraphX_Internal::LoggerManager::Instance().LogMessage("Engine", "Starting Engine...");
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

    void Engine::Render(SceneSystem& sceneSystem, float alpha)
    {
        _m_window->FillCanvas(CHAR_INFO{ _m_window->s_pixel, 6 });
        ConsoleGraphX_Internal::Renderer::DrawSprites(sceneSystem, alpha);
        _m_window->DrawScreen();

        ConsoleGraphX_Internal::CGXProfiler::Instance().DisplayMetrics();
    }

};
