#include <chrono>
#include "application.h"
#include "console_handler.h"


namespace ConsoleGraphX
{
    Application::Application(): _m_engine(235,158, 3, 3)
    {
        _m_sceneSystem.Initialize();

        ConsoleHandler::RegisterApplication(this);
        ConsoleHandler::SetHandler();
    }

    void Application::Run()
    {
        const float targetUpdateRate = 1.0f / 60.0f; // Fixed timestep of 60 updates per second
        float accumulator = 0.0f;
        int framesPerSecond = 0;
        int frameCounter = 0;
        float fpsTimeCounter = 0.0f;

        auto previousTime = std::chrono::high_resolution_clock::now();

        _m_engine.Start();
        while (_m_engine.IsRunning())
        {
            // Get the current time and calculate delta time
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> deltaTime = currentTime - previousTime;
            previousTime = currentTime;

            // Add delta time to the accumulator
            accumulator += deltaTime.count();

            // Update FPS counter
            fpsTimeCounter += deltaTime.count();
            frameCounter++;

            // Fixed timestep for updating systems
            while (accumulator >= targetUpdateRate)
            {
                _m_engine.UpdateSystems(targetUpdateRate);
                accumulator -= targetUpdateRate;
            }

            // Render the frame
            float alpha = accumulator / targetUpdateRate;
            _m_engine.Render(alpha);

            // FPS update (once per second)
            if (fpsTimeCounter >= 1.0f)
            {
                framesPerSecond = frameCounter;
                frameCounter = 0;
                fpsTimeCounter = 0.0f;

                _m_engine.UpdateFPS(framesPerSecond); // A method in the engine to set the FPS display
            }
        }

        Shutdown();
    }

    void Application::Shutdown()
    {

        _m_sceneSystem.ShutDown();
        _m_engine.Shutdown();

    }

    void Application::OnConsoleClose()
    {
        Shutdown();
    }
};
