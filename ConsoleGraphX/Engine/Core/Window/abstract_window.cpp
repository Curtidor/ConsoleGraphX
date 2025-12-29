#include "PCH_CGX.h"
#include <windowsx.h>
#include "Engine\Core\Window\abstract_window.h"
#include "Engine\Core\Logger\logger_manager.h"
#include "../WinCore/WinCore.h"

namespace ConsoleGraphX
{
    AbstractWindow::AbstractWindow(const std::string& windowName, ConsoleGraphX_Internal::Screen screen)
		: _m_windowName(windowName), _m_windowHWND(NULL), _m_closeEvent(INVALID_HANDLE_VALUE), m_deferWindowPosCountWithWrongSize(0), _m_screen(std::move(screen))
    {}

    void AbstractWindow::SetupWindow()
    {
		SetupWindowImpl();

		UpdateWindowSize();
    }

    void AbstractWindow::UpdateWindowSize()
    {
        WinCore::WindowRectPx rectPX{};
        WinCore::GetWindowOuterRectPx(GetHWND(), rectPX);
        _m_widthPx = rectPX.Width();
        _m_heightPx = rectPX.Height();
    }
   
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

    void AbstractWindow::MoveWindow(int x, int y)
    {
		WinCore::MoveWindowNoSize(GetHWND(), std::max<int>(x, 0), y);
    }

    void AbstractWindow::SetHWND(HWND windowHWND)
    {
        _m_windowHWND = windowHWND;
    }

    Vector2i AbstractWindow::GetTargetWindowSize()
    {
		return { _m_widthPx, _m_heightPx };
    }
    bool AbstractWindow::SetScreenSize(uint16_t width, uint16_t height, uint16_t fontWidth, uint16_t fontHeigh)
    {
        bool status = _m_screen.SetNewScreenSize(width, height, fontWidth, fontHeigh);

        UpdateWindowSize();

		return status;
    }
  
}
