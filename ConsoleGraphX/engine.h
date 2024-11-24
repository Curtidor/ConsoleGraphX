#pragma once
#include "logger.h"
#include "screen.h"
#include "system_manager.h"
#include "scene_system.h"

namespace ConsoleGraphX
{

    class Engine
    {
    private:
        ConsoleGraphX::Window* _m_window;
        ConsoleGraphX_Internal::Logger _m_logger;
        ConsoleGraphX_Internal::SystemManager _m_systemManager;


    public:
        Engine();
       
        void AttachWindow(Window* window);
        void WarmUp();
        void Shutdown();

        void UpdateSystems(float deltaTime);
        void Render(SceneSystem& sceneSystem, float alpha);
    };
};
