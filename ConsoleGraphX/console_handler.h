#pragma once
#pragma once
#include <Windows.h>

namespace ConsoleGraphX
{
    class Engine;

}

class ConsoleHandler 
{
public:
    static void SetHandler();
    static void RegisterEngine(ConsoleGraphX::Engine* engine);

private:
    static ConsoleGraphX::Engine* _instance;
    static BOOL WINAPI ConsoleHandlerRoutine(DWORD ctrlType);
};
