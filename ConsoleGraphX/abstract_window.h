#pragma once
#include <string>
#include <windows.h>
#include <iostream>
#include "events.h"

namespace ConsoleGraphX
{
    struct WindowPositionData
    {
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height;
    };

    class AbstractWindow
    {
    public:
        CGXEventArgs<unsigned short, unsigned short> OnWindowCreated;
        CGXEventArgs<unsigned short, unsigned short> OnWindowResized;

        CGXEventArgs<unsigned int> OnKeyPressed;
        CGXEventArgs<unsigned int> OnKeyReleased;

        CGXEventArgs<AbstractWindow*> OnWindowDestroyed;

        CGXEvent OnWindowFocusGained;
        CGXEvent OnWindowFocusLost;

    protected:
        HWND _m_windowHWND;
        std::string _m_windowName;

    public:
        explicit AbstractWindow(const std::string& windowName);
        virtual ~AbstractWindow() = default;

        virtual void Destroy() = 0;

        std::string_view GetWindowName() const;
        std::string& GetWindowNameR();

        WindowPositionData GetWindowPosition();
        const HWND GetHWND();

        void SetWindowPosition(short x, short y);
        void ResizeWindow(unsigned short newWidth, unsigned short newHeight);

    private:
        void _TrySetWindowHandle();
    };
}
