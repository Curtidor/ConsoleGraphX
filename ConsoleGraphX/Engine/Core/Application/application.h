#pragma once
#include <atomic>
#include <queue>
#include "Engine\Core\Window\abstract_window.h"
#include "Engine\Core\Event\events.h"
#include "Engine\Core\Application\engine.h"
#include "Engine\Systems\scene_system.h"
#include "../External/TermLog/Client/client.h"

namespace ConsoleGraphX
{

    enum class ApplicationState
    {
        Running,       // The application is actively running
        ShuttingDown,  // The application is in the process of shutting down
        Stopped        // The application has fully stopped
    };


    class Application
    {
    public:
        Application();
        void WarmUp(SceneSystem& sceneSystem);
        void Run(SceneSystem& sceneSystem);
        void Shutdown();    
        void OnConsoleClose(AbstractWindow* window);

    public:
        CGXEvent OnClose;
        Engine m_engine;

    private:
        std::condition_variable _m_condition;
        std::mutex _m_mutex;
        std::mutex _m_taskMutex;
        std::queue<std::function<void()>> _m_task;
        bool _m_mainLoopExited = false;
        bool _m_isRunning = false;

        ApplicationState _m_state;
    };

    // please forgive me its been days of trying to fix the closing bug cant be bothered this is hopefully temp (it never is)
#if MIN_BUILD == 1
    extern Application* g_app;
#endif


};
