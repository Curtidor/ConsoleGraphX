#pragma once
#include <string>
#include <windows.h>
#include <iostream>
#include "Engine\Math\vector2.h"
#include "Engine\Core\Event\events.h"

namespace ConsoleGraphX
{
    struct WindowPositionData
    {
        int64_t x;
        int64_t y;
        int64_t width;
        int64_t height;
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

        uint32_t m_deferWindowPosCountWithWrongSize;

    protected:
        // derived classes can set this when they know the handle
        mutable std::atomic<HWND> _m_windowHWND{ nullptr };

        HANDLE _m_closeEvent;
        std::string _m_windowName;

    protected:

        // default: nothing to do
        virtual void TryResolveHWND() const noexcept {}

    public:
        explicit AbstractWindow(const std::string& windowName);
        virtual ~AbstractWindow() = default;

        virtual void SetupWindow() = 0;
        virtual void Destroy() = 0;

        bool OpenCloseEvent();

        HWND GetHWND() const noexcept;
        bool HasHWND() const noexcept { return GetHWND() != nullptr; }

        std::string& GetWindowNameR();
        const std::string_view GetWindowName() const;
        const WindowPositionData GetWindowPosition() const;
        const HANDLE GetCloseEventHandle() const;

        void SetWindowPosition(int x, int y);
        void SetHWND(HWND windowHWND);
        void ResizeWindow(unsigned short newWidth, unsigned short newHeight, bool triggerEvent = true);
        virtual Vector2 GetTargetWindowSize() = 0;
        virtual Vector2 GetTargetWindowSizeInPixels() = 0;
    };
}
