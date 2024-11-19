#pragma once
#include "logger.h"
#include "screen.h"
#include "system_manager.h"
#include "scene_system.h"

namespace ConsoleGraphX
{

    enum ENGINE_MODE
    {
        EDITOR,
        STANDALONE
    };

    class Engine
    {
    private:

        bool _m_isRunning;
        
        ConsoleGraphX::Window* _m_window;
        ConsoleGraphX_Internal::Logger _m_logger;
        ConsoleGraphX_Internal::SystemManager _m_systemManager;


    public:
        Engine(short screen_width, short screen_height, short pixel_width, short pixel_height);
       
        void Initialize();
        void WarmUp();
        void Start();
        void Shutdown();
        bool IsRunning() const;

        void UpdateSystems(float deltaTime);
        void Render(SceneSystem& sceneSystem, float alpha);
    };
};
