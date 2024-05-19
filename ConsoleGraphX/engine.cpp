#include <string>
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
    ConsoleGraphX_Interal::SystemManager* Engine::_s_systemManager = nullptr;
    ConsoleGraphX_Interal::Debugger* Engine::_s_debugger = nullptr;

    bool Engine::_s_isRunning = false;

    void Engine::InitializeEngine(short screen_width, short screen_height, short pixel_width, short pixel_height, ConsoleGraphX_Interal::Debugger& debugger)
    {
        _s_debugger = &debugger;

        int debugger_height = _s_debugger->GetMaxMessages();

        ConsoleGraphX_Interal::Screen* screen = new ConsoleGraphX_Interal::Screen(screen_width, screen_height, debugger_height, pixel_width, pixel_height);
        ConsoleGraphX_Interal::Screen::SetActiveScreen_A(screen);
        
        _s_systemManager = new ConsoleGraphX_Interal::SystemManager();
        _s_systemManager->RegisterSystem<ScriptSystem>();
        _s_systemManager->RegisterSystem<ConsoleGraphX_Interal::SpriteSystem>();
        _s_systemManager->RegisterSystem<PlayerControllerSystem>();
        _s_systemManager->RegisterSystem<CameraSystem>();
    }

    void Engine::Run()
    {
        if (_s_isRunning)
            return;

        ScriptSystem::WarmUp();

        ConsoleGraphX_Interal::Screen* active_screen = ConsoleGraphX_Interal::Screen::GetActiveScreen_A();
      
        FrameRateController frameRateController(60);
        FrameRateController updateTimer;
        std::chrono::nanoseconds deltaTime(0);


        // NOTE THIS TIMING CODE IS A PLACE HOLDER AND IS HERE FOR TESTING
        while (true) {
            frameRateController.CaptureFrame();  // Capture start time of the frame
            updateTimer.StartCapture();  // Start timing the update and draw operations
            
            deltaTime = frameRateController.GetDeltaTimeMS();
            
            _Update(1.0f/60);  // Update with delta time in seconds

            active_screen->DrawScreen();
            active_screen->MemFillScreen(0);

            active_screen->SetConsoleName("FPS " + std::to_string(frameRateController.GetFramesPerSecond()));

            updateTimer.EndCapture();  // End timing the update and draw operations

            deltaTime = updateTimer.GetDeltaTimeMS();  // Get the time taken for update and draw operations

            frameRateController.WaitForNextFrame(std::chrono::duration_cast<std::chrono::milliseconds>(deltaTime + frameRateController.GetDeltaTimeMS()));  // Wait for the next frame if needed
        }

    }

    void Engine::_Update(float deltaTime)
    {
        InputSystem::GetPressedKeys();
        InputSystem::UpdateMousePosition();

        _s_systemManager->Update(deltaTime);

        ConsoleGraphX_Interal::RenderSystem::DrawSprites(ConsoleGraphX_Interal::SpriteSystem::GetEntitySprites());

        SceneSystem::GetActiveScene()->DeleteEntities();

        _s_debugger->DisplayMessages();
    }
};



