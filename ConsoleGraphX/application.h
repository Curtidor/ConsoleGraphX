#pragma once
#include "engine.h"
#include "window_layout.h"
#include "scene_system.h"
#include "events.h"

namespace ConsoleGraphX
{
    class Application
    {
    public:
        Application(short screenWidth, short screenHeight, short pixelWidth, short pixelHeight);
        void Initialize();
        void WarmUp();
        void Run(SceneSystem& sceneSystem);
        void Shutdown();    
        void OnConsoleClose();

    public:
        CGXEvent OnClose;

    private:

        std::condition_variable _m_mainLoopCondition;
        std::mutex _m_mutex;
        bool _m_mainLoopExited = false;

        Engine _m_engine;    
        WindowLayout _m_layout;
    };

};
