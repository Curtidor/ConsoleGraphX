#include "PCH_CGX.h"
#include <windowsx.h>
#include "Engine\Core\Window\abstract_window.h"
#include "Engine\Core\Logger\logger_manager.h"

namespace ConsoleGraphX
{
    AbstractWindow::AbstractWindow(const std::string& windowName)
		: _m_windowName(windowName), _m_windowHWND(NULL), _m_closeEvent(INVALID_HANDLE_VALUE), m_deferWindowPosCountWithWrongSize(0)
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
        positionData.x = sizeData.left;
        positionData.y = sizeData.top;
        positionData.width = sizeData.right - sizeData.left;
        positionData.height = sizeData.bottom - sizeData.top;

        return positionData;
    }

    HWND AbstractWindow::GetHWND() const noexcept 
    {
        HWND h = _m_windowHWND.load(std::memory_order_acquire);
        if (!h) 
        {
            TryResolveHWND();  // let derived try to fill it
            h = _m_windowHWND.load(std::memory_order_acquire);
        }
        return h;
    }

    const HANDLE AbstractWindow::GetCloseEventHandle() const
    {
        return _m_closeEvent;
    }

    void AbstractWindow::SetWindowPosition(int x, int y)
    {
        const Vector2 size = GetTargetWindowSizeInPixels();
        SetWindowPos(_m_windowHWND, NULL, std::max<int>(x, 0), y, size.x, size.y, SWP_NOZORDER | SWP_SHOWWINDOW);
    }

    void AbstractWindow::SetHWND(HWND windowHWND)
    {
        _m_windowHWND = windowHWND;
    }

    void AbstractWindow::ResizeWindow(unsigned short newWidth, unsigned short newHeight, bool triggerEvent)
    {
        CrossProcessWindow* cpwWindow = dynamic_cast<CrossProcessWindow*>(this);
        if (cpwWindow)
        {
            cpwWindow->SetNewScreenSize(newWidth, newHeight, cpwWindow->GetPixelWidth(), cpwWindow->GetPixelHeight());
        }   

        Window* window = dynamic_cast<Window*>(this);
        if (window)
        {
            window->SetNewScreenSize(newWidth, newHeight, window->GetPixelWidth(), window->GetPixelHeight());
        }

        if (triggerEvent)
            OnWindowResized.InvokeNF(newWidth, newHeight);
    }


    

}
