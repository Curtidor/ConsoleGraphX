#pragma once
#include "Engine\Core\Logger\logger.h"
#include "Engine\Graphics\ScreenGraphics\screen.h"
#include "Engine\Core\Application\system_manager.h"
#include "Engine\Systems\scene_system.h"
#include "Engine\Core\Concurrency\thread_manager.h"

namespace ConsoleGraphX
{

    class Engine
    {
    private:
    #if MIN_BUILD == 1
        ConsoleGraphX_Internal::Screen* _m_window;
    #else
        std::shared_ptr<Window> _m_window;
    #endif
        ConsoleGraphX_Internal::Logger _m_logger;
        ConsoleGraphX_Internal::SystemManager _m_systemManager;

    public:
        ConsoleGraphX_Internal::ThreadManager m_threadManager;


    public:
        Engine();
       
        void AttachWindow(std::shared_ptr<Window> window);
        void WarmUp(SceneSystem& sceneSystem);
        void Shutdown();

        void UpdateSystems(float deltaTime, SceneSystem& sceneSystem);
        void Render(SceneSystem& sceneSystem, float alpha);

        ConsoleGraphX_Internal::SystemManager& GetSystemManager();
    };
};
