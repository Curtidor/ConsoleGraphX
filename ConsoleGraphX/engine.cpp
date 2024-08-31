#include <string>
#include <iostream>
#include <chrono>
#include "engine.h"
#include "frame_rate_controller.h"
#include "debugger.h"
#include "system_manager.h"
#include "input_system.h"
#include "player_controller_system.h"
#include "render_system.h"
#include "scene_system.h"
#include "screen.h"
#include "script_system.h"
#include "sprtite_system.h"
#include "camera_system.h"

namespace ConsoleGraphX
{
    ConsoleGraphX_Internal::SystemManager* Engine::_systemManager = nullptr;
    ConsoleGraphX_Internal::Debugger* Engine::_debugger = nullptr;
    bool Engine::_m_isRunning = false;

    void Engine::Initialize(short screen_width, short screen_height, short pixel_width, short pixel_height, ConsoleGraphX_Internal::Debugger& debugger)
    {
        _debugger = &debugger;

        // the screen class will manage the heap allocated screen
        ConsoleGraphX_Internal::Screen* screen = new ConsoleGraphX_Internal::Screen(screen_width, screen_height, pixel_width, pixel_height);
        ConsoleGraphX_Internal::Screen::SetActiveScreen_A(screen);

        _systemManager = new ConsoleGraphX_Internal::SystemManager();
        _systemManager->RegisterSystem<ScriptSystem>();
        _systemManager->RegisterSystem<ConsoleGraphX_Internal::SpriteSystem>();
        _systemManager->RegisterSystem<PlayerControllerSystem>();
        _systemManager->RegisterSystem<CameraSystem>();
    }

    void Engine::Shutdown()
    {
        if (_debugger)
        {
            _debugger->LogMessage("closing...");
            _debugger->LogMessage("End_Receiver_CGX");
        }

        delete _systemManager;
    }

    void Engine::Run()
    {
        if (_m_isRunning)
            return;

        _m_isRunning = true;

        // Set the console control handler
        if (!SetConsoleCtrlHandler(&_CleanUp, TRUE))
        {
            std::cerr << "Failed to set console control handler" << std::endl;
            return;
        }

        ScriptSystem::WarmUp();

        ConsoleGraphX_Internal::Screen* active_screen = ConsoleGraphX_Internal::Screen::GetActiveScreen_A();

        FrameRateController frameRateController(60);
        std::chrono::nanoseconds deltaTime(0);

        while (_m_isRunning)
        {
            auto frameStartTime = std::chrono::high_resolution_clock::now(); // Start frame time capture

            _Update(std::chrono::duration<float>(deltaTime).count()); // Update with delta time in seconds

            active_screen->DrawScreen();
            active_screen->FillScreen({ ConsoleGraphX_Internal::Screen::s_pixel, 0 });

            active_screen->SetConsoleName("FPS " + std::to_string(frameRateController.GetFramesPerSecond()));

            auto frameEndTime = std::chrono::high_resolution_clock::now(); // End frame time capture
            deltaTime = frameEndTime - frameStartTime; // Calculate delta time

            frameRateController.WaitForNextFrame(std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime));

            frameRateController.CaptureFrame(); // Capture the end of the frame
        }
    }

    void Engine::_Update(float deltaTime)
    {
        InputSystem::GetPressedKeys();

        //InputSystem::UpdateMousePosition();

        if (_systemManager)
        {
            _systemManager->Update(deltaTime);
        }

        ConsoleGraphX_Internal::RenderSystem::DrawSprites();
    }

    BOOL WINAPI Engine::_CleanUp(DWORD ctrlType)
    {
        switch (ctrlType)
        {
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            _m_isRunning = false;
            Engine::Shutdown();
            return TRUE;
        default:
            return FALSE;
        }
    }
};
