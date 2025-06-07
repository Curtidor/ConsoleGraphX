#include "PCH_CGX.h"
#include "Engine\Core\Window\abstract_window.h"
#include "Engine\Core\Logger\logger_manager.h"

namespace ConsoleGraphX
{
    AbstractWindow::AbstractWindow(const std::string& windowName)
        : _m_windowName(windowName), _m_windowHWND(NULL), _m_closeEvent(INVALID_HANDLE_VALUE)
    {}

    bool AbstractWindow::OpenCloseEvent()
    {
        if (_m_closeEvent && _m_closeEvent != INVALID_HANDLE_VALUE)
            return true;

        std::string eventName = WINDOW_CLOSE_EVENT_NAME(_m_windowName);
        _m_closeEvent = OpenEventA(EVENT_ALL_ACCESS, FALSE, eventName.c_str());

        return (_m_closeEvent && _m_closeEvent != INVALID_HANDLE_VALUE);
    }   

    const std::string_view AbstractWindow::GetWindowName() const
    {
        return _m_windowName;
    }

    std::string& AbstractWindow::GetWindowNameR()
    {
        return _m_windowName;
    }

    const WindowPositionData AbstractWindow::GetWindowPosition() const
    {
        HWND windowHWND = GetHWND();

        if (windowHWND == NULL)
            return { 0, 0, 0, 0 };

        RECT sizeData{};
        if (!GetWindowRect(windowHWND, &sizeData))
        {
            std::string errorMessage = "Failed to get window rect: " + std::to_string(GetLastError());
            ConsoleGraphX_Internal::LoggerManager::Instance().LogMessage("AdjustZOrder", errorMessage);

            return { 0, 0, 0, 0 };
        }

        WindowPositionData positionData{};
        positionData.x = static_cast<uint16_t>(sizeData.left);
        positionData.y = static_cast<uint16_t>(sizeData.top);
        positionData.width = static_cast<uint16_t>(sizeData.right - sizeData.left);
        positionData.height = static_cast<uint16_t>(sizeData.bottom - sizeData.top);

        return positionData;
    }

    const HWND AbstractWindow::GetHWND() const
    {
        if (_m_windowHWND == NULL)
        {
           return FindWindowA(NULL, _m_windowName.c_str());
        }
        
        return _m_windowHWND;
    }

    const HANDLE AbstractWindow::GetCloseEventHandle() const
    {
        return _m_closeEvent;
    }

    void AbstractWindow::SetWindowPosition(int x, int y)
    {
        WindowPositionData wp = GetWindowPosition();
        SetWindowPos(_m_windowHWND, NULL, std::max<int>(x, 0), y, wp.width, wp.height, SWP_NOZORDER | SWP_SHOWWINDOW);
    }

    void AbstractWindow::SetHWND(HWND windowHWND)
    {
        _m_windowHWND = windowHWND;
    }

    void AbstractWindow::ResizeWindow(unsigned short newWidth, unsigned short newHeight)
    {
        OnWindowResized.InvokeNF(newWidth, newHeight);
    }
}
