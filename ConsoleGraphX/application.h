#pragma once
#include "engine.h"
#include "scene_system.h"
#include "events.h"

namespace ConsoleGraphX
{
    class Application
    {
    public:
        CGXEvent OnClose;

    public:
        Application();
        void Initialize();
        void WarmUp();
        void Run(SceneSystem& sceneSystem);
        void Shutdown();    
        void OnConsoleClose();

    private:

        std::condition_variable _m_mainLoopCondition;
        std::mutex _m_mutex;
        bool _m_mainLoopExited = false;
        bool _m_isRunning = false;

        Engine _m_engine;    
    };

};
