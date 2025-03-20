#pragma once
#include "Engine\Core\Logger\logger.h"
#include "Engine\Graphics\ScreenGraphics\screen.h"
#include "Engine\Core\Application\system_manager.h"
#include "Engine\Systems\scene_system.h"

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
        void WarmUp(SceneSystem& sceneSystem);
        void Shutdown();

        void UpdateSystems(float deltaTime, SceneSystem& sceneSystem);
        void Render(SceneSystem& sceneSystem, float alpha);

        ConsoleGraphX_Internal::SystemManager& GetSystemManager();
    };
};
