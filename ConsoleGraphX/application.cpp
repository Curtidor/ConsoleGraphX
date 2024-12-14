#include "PCH_CGX.h"
#include "application.h"
#include "window_manager.h"
#include "window_layout.h"
#include "profiler.h"

namespace ConsoleGraphX
{
    Application::Application()
        : _m_engine(Engine())
    {}

    void Application::WarmUp(SceneSystem& sceneSystem)
    {
        _m_engine.WarmUp(sceneSystem);
    }

    void Application::Run(SceneSystem& sceneSystem)
    {
        _m_state = ApplicationState::Running;

        const float targetUpdateRate = 1.0f / 60.0f;
        float accumulator = 0.0f;
        int framesPerSecond = 0;
        int frameCounter = 0;
        float fpsTimeCounter = 0.0f;

        auto previousTime = std::chrono::high_resolution_clock::now();

        bool notified = false;
        while (_m_state != ApplicationState::Stopped)
        {
            if (_m_state == ApplicationState::Running)
            {
                auto currentTime = std::chrono::high_resolution_clock::now();
                std::chrono::duration<float> deltaTime = currentTime - previousTime;
                previousTime = currentTime;

                accumulator += deltaTime.count();
                fpsTimeCounter += deltaTime.count();
                frameCounter++;

                while (accumulator >= targetUpdateRate && _m_state == ApplicationState::Running)
                {
                    _m_engine.UpdateSystems(targetUpdateRate, sceneSystem);
                    accumulator -= targetUpdateRate;
                }
                float alpha = accumulator / targetUpdateRate;
                _m_engine.Render(sceneSystem, alpha);
            }

            WindowManager::Instance().ProcessToCloseWindows();

            if (_m_state == ApplicationState::ShuttingDown && !notified)
            {
                {
                    std::lock_guard<std::mutex> lock(_m_mutex);
                    _m_condition.notify_one();
                }

                notified = true;
            }
        }
    }

    void Application::Shutdown()
    {
        {
            std::unique_lock<std::mutex> lock(_m_mutex);
            _m_state = ApplicationState::ShuttingDown;

            _m_condition.wait(lock);

            _m_state = ApplicationState::Stopped;

            ConsoleGraphX_Internal::LoggerManager::ShutDown();
            WindowManager::ShutDown();
           
            std::cout << "Shut down done" << std::endl;
        }
       
    }

    void Application::OnConsoleClose(AbstractWindow* window)
    {
        OnClose.Invoke();

        std::thread shutdownThread([this]() {
            Shutdown();
            });

        // detach the thread to allow it to run independently
        shutdownThread.detach();
    }
}
