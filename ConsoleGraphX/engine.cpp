#include <string>
#include <chrono>
#include "engine.h"
#include "frame_rate_controller.h"
#include "logger.h"
#include "system_manager.h"
#include "input_system.h"
#include "player_controller_system.h"
#include "render_system.h"
#include "resourcec_manager.h"
#include "screen.h"
#include "script_system.h"
#include "color.h"
#include "logger.h"
#include "console_handler.h"
#include "debugger_manager.h"

namespace ConsoleGraphX
{
    Engine::Engine(short screen_width, short screen_height, short pixel_width, short pixel_height)
        : _m_logger(ConsoleGraphX_Internal::Logger("Engine")), _m_systemManager(ConsoleGraphX_Internal::SystemManager()), _m_screen(ConsoleGraphX_Internal::Screen(screen_width, screen_height, pixel_width, pixel_height))
    {
        ConsoleGraphX_Internal::Screen::SetActiveScreen_A(&_m_screen);

        _m_systemManager.RegisterSystem<ScriptSystem>();
        _m_systemManager.RegisterSystem<PlayerControllerSystem>();

        ConsoleGraphX_Internal::DebuggerManager::Initialize();
        ConsoleGraphX_Internal::ResourceManager::Initialize();
    }

    void Engine::Shutdown()
    {
        ConsoleGraphX_Internal::DebuggerManager::ShutDown();
    }

    void Engine::OnConsoleClose()
    {
        _m_isRunning = false;
        Shutdown();
    }

    void Engine::Run()
    {
        if (_m_isRunning)
            return;

        _m_isRunning = true;

        _m_logger.LogMessage("Starting Engine.");

        ConsoleHandler::RegisterEngine(this);
        ConsoleHandler::SetHandler();

        ScriptSystem::ScriptWarmUp();

        ConsoleGraphX_Internal::Screen* active_screen = ConsoleGraphX_Internal::Screen::GetActiveScreen_A();

        FrameRateController frameRateController(60);
        std::chrono::nanoseconds deltaTime(0);
        while (_m_isRunning)
        {
            auto frameStartTime = std::chrono::high_resolution_clock::now(); // Start frame time capture

            _Update(std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime).count() / 1000.0f); // Update with delta time in seconds

            active_screen->DrawScreen();
            active_screen->FillScreen({ ConsoleGraphX_Internal::Screen::s_pixel, Color::DarkYellow });

            active_screen->SetConsoleName("FPS " + std::to_string(frameRateController.GetFramesPerSecond()));

            auto frameEndTime = std::chrono::high_resolution_clock::now(); // End frame time capture
            deltaTime = frameEndTime - frameStartTime; // Calculate delta time

            active_screen->SetConsoleName("FPS " + std::to_string(1 / deltaTime.count()));

            frameRateController.CaptureFrame(); // Capture the end of the frame

        }
        // this is here and not in shutdown as the current frame needs to finish before closing
        ConsoleGraphX_Internal::ResourceManager::ShutDown();
    }

    void Engine::_Update(float deltaTime)
    {
        InputSystem::GetPressedKeys();

        //InputSystem::UpdateMousePosition();
        _m_systemManager.Update(deltaTime);
       
        ConsoleGraphX_Internal::RenderSystem::DrawSprites();
    }
};
