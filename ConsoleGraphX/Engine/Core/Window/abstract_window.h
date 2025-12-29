#pragma once
#include <string>
#include <windows.h>
#include <iostream>
#include "Engine\Math\vector2i.h"
#include "Engine\Core\Event\events.h"
#include "Engine\Graphics\ScreenGraphics\screen.h"

namespace ConsoleGraphX
{
    struct WindowPositionData
    {
        int32_t x;
        int32_t y;
        int32_t width;
        int32_t height;
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
        CGXEventArgs<uint32_t, uint32_t> OnWindowCreated;
        CGXEventArgs<uint32_t, uint32_t> OnWindowResized;

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
        int32_t _m_widthPx;
		int32_t _m_heightPx;
        ConsoleGraphX_Internal::Screen _m_screen;

    protected:

        // default: nothing to do
        virtual void TryResolveHWND() const noexcept {}
        virtual void SetupWindowImpl() = 0;


    public:
        explicit AbstractWindow(const std::string& windowName, ConsoleGraphX_Internal::Screen screen);
        virtual ~AbstractWindow() = default;

        void SetupWindow();
        void UpdateWindowSize();
        virtual void Destroy() = 0;

        bool OpenCloseEvent();

        HWND GetHWND() const noexcept;
        bool HasHWND() const noexcept { return GetHWND() != nullptr; }

        std::string& GetWindowNameR();
        const std::string_view GetWindowName() const;
        const HANDLE GetCloseEventHandle() const;

        void MoveWindow(int x, int y);
        void SetHWND(HWND windowHWND);
        Vector2i GetTargetWindowSize();
        bool SetScreenSize(uint16_t width, uint16_t height, uint16_t fontWidth = 0, uint16_t fontHeigh = 0);
        const ConsoleGraphX_Internal::Screen& GetScreen() const { return _m_screen;}

    };
}
