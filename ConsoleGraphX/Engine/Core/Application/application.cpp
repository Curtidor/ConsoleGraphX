#include "PCH_CGX.h"
#include "Engine/Core/Application/application.h"
#include "Engine/Core/Window/window_manager.h"
#include "Engine/Core/Profiler/profiler.h"
#include "Engine/Layout/window_layout.h"


namespace ConsoleGraphX
{
#if MIN_BUILD == 1
    Application* g_app = nullptr;
#endif

    Application::Application()
        : m_engine(Engine()), _m_state(ApplicationState::Running)
    {
    }

    void Application::WarmUp(SceneSystem& sceneSystem)
    {
        m_engine.WarmUp(sceneSystem);
    }

    void Application::Run(SceneSystem& sceneSystem)
    {
        const float targetUpdateRate = 1.0f / 60.0f;
        float accumulator = 0.0f;
        int framesPerSecond = 0;
        int frameCounter = 0;
        float fpsTimeCounter = 0.0f;

        auto previousTime = std::chrono::high_resolution_clock::now();

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

        #if (MIN_BUILD == 1)
            Shutdown();
        #else
           WindowManager::Instance().ProcessToCloseWindows();
        #endif // (DEBUG) && (MIN_BUILD == 0)

           while (!_m_task.empty())
           {
               auto task = _m_task.front();
               _m_task.pop();

               if (task)
                   task();
           }

        }
    }

    void Application::Shutdown()
    {
        //OnClose.Invoke();
        
        while (!_m_task.empty())
        {
            _m_task.pop(); // remove any remaining task
        }

        _m_state = ApplicationState::ShuttingDown;
        _m_state = ApplicationState::Stopped;

        #if (MIN_BUILD == 0)
        m_engine.m_threadManager.RequestShutdownAll();
        m_engine.m_threadManager.JoinAll();

        WindowManager::ShutDown();
        ConsoleGraphX_Internal::LoggerManager::ShutDown(); 
        #endif

        m_engine.Shutdown();
        std::cout << "Shut down done" << std::endl;
    }

    void Application::OnConsoleClose(AbstractWindow* window)
    {
        _m_task.push([this]() { Shutdown(); });
    }
}
