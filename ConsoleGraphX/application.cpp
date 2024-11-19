#include "PCH_CGX.h"
#include "application.h"
#include "console_handler.h"
#include "window_manager.h"
#include "window_layout.h"
#include "profiler.h"



namespace ConsoleGraphX
{
    Application::Application(short screenWidth, short screenHeight, short pixelWidth, short pixelHeight)
    : _m_engine(Engine(screenWidth, screenHeight, pixelWidth, pixelHeight))
    {
        ConsoleHandler::RegisterApplication(this);
        ConsoleHandler::SetHandler();
    }

    void Application::Initialize()
    {
        WindowManager::Initialize();
        _m_engine.Initialize();
    }

    void Application::WarmUp()
    {
        _m_engine.WarmUp();
    }

    void Application::Run(SceneSystem& sceneSystem)
    {
        _m_engine.Start();

        const float targetUpdateRate = 1.0f / 60.0f; // Fixed timestep of 60 updates per second
        float accumulator = 0.0f;
        int framesPerSecond = 0;
        int frameCounter = 0;
        float fpsTimeCounter = 0.0f;

        auto previousTime = std::chrono::high_resolution_clock::now();

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
            _m_engine.Render(sceneSystem, alpha);

            // FPS update (once per second)
            if (fpsTimeCounter >= 1.0f)
            {
                framesPerSecond = frameCounter;
                frameCounter = 0;
                fpsTimeCounter = 0.0f;

            }

            ConsoleGraphX_Internal::INCREMENT_COUNTER("FPS", framesPerSecond);


            _m_layout.ApplyLayout();
        }

        // signal that the main loop has exited
        {
            std::lock_guard<std::mutex> lock(_m_mutex);
            _m_mainLoopExited = true;
        }
        _m_mainLoopCondition.notify_one();
    }

    void Application::Shutdown()
    {
        _m_engine.Shutdown();
        
        // wait for the main loop to exit before shutting down the WindowManager
        // this way any final render calls will have a valid screen to draw to
        {
            std::unique_lock<std::mutex> lock(_m_mutex);
            _m_mainLoopCondition.wait(lock, [this]() { return _m_mainLoopExited; });
        }

        WindowManager::ShutDown();
    }

    void Application::OnConsoleClose()
    {
        OnClose.Invoke();
        Shutdown();
    }
};
