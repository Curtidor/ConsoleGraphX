#pragma once
#include <atomic>
#include "logger.h"
#include "screen.h"
#include "system_manager.h"

namespace ConsoleGraphX
{
    class Engine
    {
    private:

        std::atomic<bool> _m_isRunning = false;
        ConsoleGraphX_Internal::Screen _m_screen;
        ConsoleGraphX_Internal::Logger _m_logger;
        ConsoleGraphX_Internal::SystemManager _m_systemManager;

    private:
        void _Update(float deltaTime);

    public:
        Engine(short screen_width, short screen_height, short pixel_width, short pixel_height);

        void Shutdown();
        void OnConsoleClose();
        void Run();

    };
};
