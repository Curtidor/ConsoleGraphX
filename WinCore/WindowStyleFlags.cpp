#include "PCH_WCore.h"
#include "WindowStyleFlags.h"

namespace WinCore
{
    bool ApplyWindowStyles_NoResize(HWND hWnd, WindowStyleFlags flags) noexcept
    {
        if (!hWnd) return false;

        LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
        if (!style && GetLastError() != 0) return false;

        if (HasFlag(flags, WindowStyleFlags::NoResize))   style &= ~WS_SIZEBOX;
        if (HasFlag(flags, WindowStyleFlags::NoMaximize)) style &= ~WS_MAXIMIZEBOX;
        if (HasFlag(flags, WindowStyleFlags::NoMinimize)) style &= ~WS_MINIMIZEBOX;

        if (HasFlag(flags, WindowStyleFlags::Borderless))
        {
            // OVERLAPPEDWINDOW includes caption/thickframe/etc. Don’t nuke the whole thing blindly.
            style &= ~(WS_CAPTION | WS_THICKFRAME);
        }

        if (HasFlag(flags, WindowStyleFlags::NoTitleBar))
        {
            style &= ~(WS_CAPTION | WS_SYSMENU);
        }

        if (HasFlag(flags, WindowStyleFlags::NoSystemMenu))
        {
            style &= ~WS_SYSMENU;
        }

        SetLastError(0);
        if (!SetWindowLongPtr(hWnd, GWL_STYLE, style) && GetLastError() != 0)
            return false;

        // Apply non-client recalculation; NO SIZE, NO MOVE.
        if (!SetWindowPos(
            hWnd, nullptr,
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED))
        {
            return false;
        }

        return true;
    }
}

