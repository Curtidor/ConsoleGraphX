#pragma once
#include <string>
#include <windows.h>
#include <iostream>
#include "Engine\Core\Event\events.h"

namespace ConsoleGraphX
{
    struct WindowPositionData
    {
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height;
    };

    /**
    * @brief Generates the name of the "CloseEvent" for a given window.
    *
    * This macro is used to construct the event name by appending "CloseEvent"
    * to the given window name. The macro is preferred here for simplicity and
    * minimal overhead since this operation is only performed during window
    * initialization, not during runtime-critical sections.
    *
    * If the event naming logic becomes more complex or is used frequently in
    * performance-sensitive areas, consider migrating this macro to a utility
    * function for better maintainability and type safety.
    *
    * @param windowName The name of the window (std::string).
    * @return The event name as a std::string.
    */
    #define WINDOW_CLOSE_EVENT_NAME(windowName) ((windowName) + std::string("CloseEvent"))


    class AbstractWindow
    {
    public:
        CGXEventArgs<unsigned short, unsigned short> OnWindowCreated;
        CGXEventArgs<unsigned short, unsigned short> OnWindowResized;

        CGXEventArgs<unsigned int> OnKeyPressed;
        CGXEventArgs<unsigned int> OnKeyReleased;

        CGXEventArgs<AbstractWindow*> OnWindowDestroyed;
        CGXEventArgs<AbstractWindow*> OnWindowClosed;

        CGXEvent OnWindowFocusGained;
        CGXEvent OnWindowFocusLost;

    protected:
        HWND _m_windowHWND;
        HANDLE _m_closeEvent;
        std::string _m_windowName;

    public:
        explicit AbstractWindow(const std::string& windowName);
        virtual ~AbstractWindow() = default;

        virtual void SetupWindow() = 0;
        virtual void Destroy() = 0;

        bool OpenCloseEvent();

        std::string& GetWindowNameR();
        const std::string_view GetWindowName() const;
        const WindowPositionData GetWindowPosition() const;
        const HWND GetHWND() const;
        const HANDLE GetCloseEventHandle() const;

        void SetWindowPosition(int x, int y);
        void SetHWND(HWND windowHWND);
        void ResizeWindow(unsigned short newWidth, unsigned short newHeight);

    };
}
