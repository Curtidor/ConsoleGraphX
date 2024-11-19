#pragma once
#include <Windows.h>
#include "application.h"


class ConsoleHandler 
{
public:
    static void SetHandler();
    static void RegisterApplication(ConsoleGraphX::Application* engine);

private:
    static inline ConsoleGraphX::Application* _s_instance = nullptr;

private:
    static BOOL WINAPI ConsoleHandlerRoutine(DWORD ctrlType);
};
