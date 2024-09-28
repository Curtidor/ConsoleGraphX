#include "console_handler.h"
#include "engine.h"


ConsoleGraphX::Engine* ConsoleHandler::_instance = nullptr;

void ConsoleHandler::SetHandler() 
{
    SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);
}

void ConsoleHandler::RegisterEngine(ConsoleGraphX::Engine* engine) 
{
    _instance = engine;
}

BOOL WINAPI ConsoleHandler::ConsoleHandlerRoutine(DWORD ctrlType) 
{
    if (_instance && (ctrlType == CTRL_CLOSE_EVENT || ctrlType == CTRL_LOGOFF_EVENT || ctrlType == CTRL_SHUTDOWN_EVENT)) 
    {
        _instance->OnConsoleClose();
        return TRUE;
    }
    return FALSE;
}
