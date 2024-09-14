#pragma once
#include "debugger.h"
#include "system_manager.h"

namespace ConsoleGraphX
{
    class Engine
    {
    private:
        static inline ConsoleGraphX_Internal::SystemManager* _systemManager = nullptr;
        static inline ConsoleGraphX_Internal::Debugger* _debugger = nullptr;
        static inline bool _m_isRunning = false;

    private:
        static BOOL WINAPI _CleanUp(DWORD ctrlType);
        static void _Update(float deltaTime);

    public:
        static void Initialize(short screen_width, short screen_height, short pixel_width, short pixel_height, ConsoleGraphX_Internal::Debugger& debugger);
        static void Run();
        static void Shutdown();
    };
};
