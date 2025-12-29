#include "PCH_CGX.h"
#include "Engine/Core/Application/application.h"
#include "Engine/Core/Window/window_manager.h"
#include "Engine/Core/Profiler/profiler.h"
#include "Engine/Core/Window/Layout/window_layout.h"
#include "Engine/termlog.h"
#include "WinCore.h"


namespace ConsoleGraphX
{
#if MIN_BUILD == 1
    Application* g_app = nullptr;
#endif

    Application::Application()
        : m_engine(Engine()), _m_state(ApplicationState::Running)
    {
#if TERMLOG == 1
        WinCore::CGXCreateProcess("LogServer.exe", std::vector<std::string>());
        Sleep(100); // give it a moment to start
        g_logClient.Connect("CGXLog");
#else

#endif // TermLog
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

        const std::vector<std::shared_ptr<CrossProcessWindow>>& crossProcWindows = WindowManager::Instance().GetAllCrossProcessWindows();

        auto previousTime = std::chrono::high_resolution_clock::now();
        while (_m_state != ApplicationState::Stopped)
        {
            for (const auto& window : crossProcWindows)
            {
                window.get()->PollInput();
			}

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
        #endif

           for (;;)
           {
               std::function<void()> task;
               {
                   std::lock_guard<std::mutex> lock(_m_taskMutex);
                   if (_m_task.empty())
                       break;
                   task = std::move(_m_task.front());
                   _m_task.pop();
               }
               if (task) task();
           }

           sceneSystem.GetActiveScene()->CleanUpDeadEntities();
           m_engine.GetSystemManager().CallSystems(ConsoleGraphX_Internal::EndOfFrame);

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
        {
            std::lock_guard<std::mutex> lock(_m_taskMutex);
            _m_task.push([this]() { Shutdown(); });
        }
    }
}
