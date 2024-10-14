#pragma once
#include "engine.h"
#include "scene_system.h"

namespace ConsoleGraphX
{
    class Application
    {
    public:
        Application();
        void Run();          // Runs the main game loop
        void Shutdown();     // Gracefully shuts down the application
        void OnConsoleClose();

    private:
        Engine _m_engine;      // The core engine handling systems and rendering
        SceneSystem _m_sceneSystem; // Handles scenes (like MainScene)
    };

};
