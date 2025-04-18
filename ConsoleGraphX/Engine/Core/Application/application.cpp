#include "PCH_CGX.h"
#include "Engine\Core\Application\application.h"
#include "Engine\Core\Window\window_manager.h"
#include "Engine\Core\Profiler\profiler.h"
#include "Engine\Layout\window_layout.h"


namespace ConsoleGraphX
{
    Application::Application()
        : m_engine(Engine()), _m_state(ApplicationState::Running)
    {}

    void Application::WarmUp(SceneSystem& sceneSystem)
    {
        m_engine.WarmUp(sceneSystem);
    }

    void Application::Run(SceneSystem& sceneSystem, std::atomic<bool>* shutdownSignal)
    {
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
                    m_engine.UpdateSystems(targetUpdateRate, sceneSystem);
                    accumulator -= targetUpdateRate;
                }
                float alpha = accumulator / targetUpdateRate;
                m_engine.Render(sceneSystem, alpha);
            }

            WindowManager::Instance().ProcessToCloseWindows();

            if (_m_state == ApplicationState::ShuttingDown && !notified)
            {
                {
                    std::lock_guard<std::mutex> lock(_m_mutex);
                    _m_condition.notify_one();
                }

                if (shutdownSignal)
                {
                    *shutdownSignal = true;
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
