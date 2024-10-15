#pragma once
#include "engine.h"
#include "scene_system.h"

namespace ConsoleGraphX
{
    class Application
    {
    public:
        Application();
        void Initialize();
        void WarmUp();
        void Run();        
        void Shutdown();    
        void OnConsoleClose();

    private:
        Engine _m_engine;    
        SceneSystem _m_sceneSystem;
    };

};
