#include "engine.h"

System* Engine::_m_system = nullptr;
Debugger* Engine::_m_debugger = nullptr;

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
    _m_system->RegisterSystem<PhysicsSystem>();

}

void Engine::Run()
{
    if (_m_is_running)
        return;

    Screen* active_screen = Screen::GetActiveScreen();
    int fps = 0;
    int frameCount = 0;
    auto lastTime = std::chrono::high_resolution_clock::now();

    Screen* screen = Screen::GetActiveScreen();

    ScriptSystem::WarmUp();

    while (true)
    {
        InputSystem::GetPressedKeys();
        InputSystem::UpdateMousePosition();
        RenderSystem::DrawSprites_SS(SpriteSystem::GetEntitySprites());

        if (_m_debugger != nullptr)
            _m_debugger->DisplayMessages();

        // call update on all the systems
        _m_system->Update();

        active_screen->DrawScreen();
        active_screen->FillScreen({ Screen::pixel, 0 });

        // Calculate FPS
        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime);

        if (elapsedTime.count() >= 1)
        {
            fps = frameCount;
            frameCount = 0;
            lastTime = currentTime;
        }

        active_screen->SetConsoleName("FPS: " + std::to_string(fps));
        
        Vector2 mouse_pos = InputSystem::GetMousePosition();
        //_m_debugger->LogMessage(std::to_string(mouse_pos.x / screen->GetPixelWidth()) + " : " + std::to_string(mouse_pos.y / screen->GetPixelHeight()));

    }
}