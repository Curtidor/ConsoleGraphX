#include "engine.h"

System* Engine::_m_system = nullptr;
Debugger* Engine::_m_debugger = nullptr;

// Define the variables for delta time calculation
std::chrono::high_resolution_clock::time_point Engine::_m_prev_frame_time;
std::chrono::high_resolution_clock::time_point Engine::_m_current_frame_time;

float Engine::_m_delta_time = 0;
bool Engine::_m_is_running = false;

void Engine::InitializeEngine(short screen_width, short screen_height, short pixel_width, short pixel_height, Debugger* debugger)
{
    int debugger_height = debugger != nullptr ? debugger->GetMaxMessages() : 0;

    Screen* screen = new Screen(screen_width, screen_height, debugger_height, pixel_width, pixel_height);
    screen->FillScreen({ Screen::pixel, 0 });

    _m_debugger = debugger;

    _m_system = new System();
    _m_system->RegisterSystem<ScriptSystem>();
    _m_system->RegisterSystem<SpriteSystem>();
    _m_system->RegisterSystem<PlayerControllerSystem>();

    // Initialize the previous frame time to the current time
    _m_prev_frame_time = std::chrono::high_resolution_clock::now();
}

void Engine::Run()
{
    if (_m_is_running)
        return;

    // Initialize all the scripts
    ScriptSystem::WarmUp();

    Screen* active_screen = Screen::GetActiveScreen_A();
    while (true)
    {
        // Calculate delta time
        _m_current_frame_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> frame_duration = _m_current_frame_time - _m_prev_frame_time;

        _m_delta_time = frame_duration.count() * 10;
        _m_prev_frame_time = _m_current_frame_time;

        InputSystem::GetPressedKeys();
        InputSystem::UpdateMousePosition();
        RenderSystem::DrawSprites(SpriteSystem::GetEntitySprites());

        // Call update on all the systems
        _m_system->Update(_m_delta_time);

        if (_m_debugger != nullptr)
            _m_debugger->DisplayMessages();
         

        UpdateFPS();

        active_screen->DrawScreen();
        active_screen->FillScreen({ Screen::pixel, 0 });

       //Sleep(4);

        _m_debugger->LogMessage(std::to_string(_m_delta_time));


    }
}

void Engine::UpdateFPS()
{
    static int frame_count = 0;
    static auto last_time = std::chrono::high_resolution_clock::now();

    frame_count++;
    auto current_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - last_time);

    if (elapsed_time.count() >= 1)
    {
        int fps = frame_count;
        frame_count = 0;
        last_time = current_time;

        Screen* active_screen = Screen::GetActiveScreen_A();
        active_screen->SetConsoleName("FPS: " + std::to_string(fps));
    }
}

const float Engine::DeltaTime()
{
    return Engine::_m_delta_time;
}
