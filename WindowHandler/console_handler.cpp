#include "PCH_WCore.h"
#include "console_handler.h"


std::function<void()> ConsoleHandler::_m_callback = nullptr;

void ConsoleHandler::SetHandler()
{
    SetConsoleCtrlHandler(_ConsoleHandlerRoutine, TRUE);
}

void ConsoleHandler::RegisterCloseCallback(const std::function<void()>& callback)
{
    _m_callback = callback;
}

BOOL WINAPI ConsoleHandler::_ConsoleHandlerRoutine(DWORD ctrlType)
{
    if (_m_callback &&
        (ctrlType == CTRL_CLOSE_EVENT ||
            ctrlType == CTRL_LOGOFF_EVENT ||
            ctrlType == CTRL_SHUTDOWN_EVENT))
    {
        _m_callback();
        return TRUE;
    }
    return FALSE;
}
