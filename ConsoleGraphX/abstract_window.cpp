#include "PCH_CGX.h"
#include "abstract_window.h"

namespace ConsoleGraphX
{
    AbstractWindow::AbstractWindow(const std::string& windowName)
        : _m_windowName(windowName), _m_windowHWND(NULL)
    {}

    std::string_view AbstractWindow::GetWindowName() const
    {
        return _m_windowName;
    }

    std::string& AbstractWindow::GetWindowNameR()
    {
        return _m_windowName;
    }

    WindowPositionData AbstractWindow::GetWindowPosition()
    {
        if (_m_windowHWND == NULL)
        {
            _TrySetWindowHandle();
        }

        if (_m_windowHWND == NULL)
            return { 0, 0, 0, 0 };

        RECT sizeData{};
        if (!GetWindowRect(_m_windowHWND, &sizeData))
        {
            std::cerr << "Failed to get window rect: " << GetLastError() << std::endl;
            return { 0, 0, 0, 0 };
        }

        WindowPositionData positionData{};
        positionData.x = static_cast<uint16_t>(sizeData.left);
        positionData.y = static_cast<uint16_t>(sizeData.top);
        positionData.width = static_cast<uint16_t>(sizeData.right - sizeData.left);
        positionData.height = static_cast<uint16_t>(sizeData.bottom - sizeData.top);

        return positionData;
    }

    const HWND AbstractWindow::GetHWND()
    {
        // if users want a gaurenteed handle, they must implment some kind of retry logic on their end,
        // assumming this function returns a bad HWND, which is possible if calling this function
        // before the console spawn
        if (_m_windowHWND == nullptr)
        {
            _m_windowHWND = FindWindowA(NULL, _m_windowName.c_str());
        }
        
        return _m_windowHWND;
    }

    void AbstractWindow::SetWindowPosition(short x, short y)
    {
        WindowPositionData wp = GetWindowPosition();
        SetWindowPos(_m_windowHWND, NULL, std::max<short>(x, 0), y, wp.width, wp.height, SWP_NOZORDER | SWP_SHOWWINDOW);
    }

    void AbstractWindow::ResizeWindow(unsigned short newWidth, unsigned short newHeight)
    {
        OnWindowResized.InvokeNF(newWidth, newHeight);
    }

    void AbstractWindow::_TrySetWindowHandle()
    {
        HWND hwnd = FindWindowA(NULL, _m_windowName.c_str());
        if (hwnd == NULL)
        {
            int error = GetLastError();
            std::cerr << "Failed to find window: " << error << std::endl;
            return;
        }
        _m_windowHWND = hwnd;
    }
}
