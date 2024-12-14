#pragma once
#include "abstract_window.h"
#include "engine.h"
#include "scene_system.h"
#include "events.h"

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
        CGXEvent OnClose;

    public:
        Application();
        void WarmUp(SceneSystem& sceneSystem);
        void Run(SceneSystem& sceneSystem);
        void Shutdown();    
        void OnConsoleClose(AbstractWindow* window);

    private:

        std::condition_variable _m_condition;
        std::mutex _m_mutex;
        bool _m_mainLoopExited = false;
        bool _m_isRunning = false;

        Engine _m_engine; 
        ApplicationState _m_state;
    };

};
