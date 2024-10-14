#include "console_handler.h"
#include "application.h"
#include "verify_macro.h"

void ConsoleHandler::SetHandler() 
{
    SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);
}

void ConsoleHandler::RegisterApplication(ConsoleGraphX::Application* app)
{
    CGX_VERIFY(app, "null application");

    _s_instance = app;
}

BOOL WINAPI ConsoleHandler::ConsoleHandlerRoutine(DWORD ctrlType) 
{
    if (_s_instance && (ctrlType == CTRL_CLOSE_EVENT || ctrlType == CTRL_LOGOFF_EVENT || ctrlType == CTRL_SHUTDOWN_EVENT)) 
    {
        _s_instance->OnConsoleClose();
        return TRUE;
    }
    return FALSE;
}
