#include "engine.h"

Engine::Engine(short screen_width, short screen_height, short pixel_width, short pixel_height, Debugger* debugger)
	: m_debugger(debugger)
{
	int debugger_height = debugger != nullptr ? debugger->GetMaxMessages() : 0;

	Screen* screen = new Screen(screen_width, screen_height, debugger_height, pixel_width, pixel_height);
	screen->FillScreen({ Screen::pixel, 0 });

	SpriteSystem* sps = new SpriteSystem();
	sps->Initalize();

	m_systems.push_back(sps);
}


void Engine::Run()
{
    Screen* active_screen = Screen::GetActiveScreen();
    int fps = 0;
    int frameCount = 0;
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (true)
    {
        InputSystem::GetPressedKeys();
        // InputSystem::UpdateMousePosition();
        RenderSystem::DrawSprites_SS(SpriteSystem::GetEntitySprites());

        if (this->m_debugger != nullptr)
            this->m_debugger->DisplayMessages();

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

        // Sleep to control the frame rate (e.g., limit to 60 FPS)
       // std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 16 milliseconds for 60 FPS
    }
}