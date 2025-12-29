#pragma once
#include <Windows.h>
#include <cstdint>

namespace WinCore
{
    enum class WindowStyleFlags : uint32_t
    {
        None = 0,
        NoResize = 1u << 0,
        NoMaximize = 1u << 1,
        NoMinimize = 1u << 2,
        Borderless = 1u << 3,
        NoTitleBar = 1u << 4,
        NoSystemMenu = 1u << 5
    };

    constexpr WindowStyleFlags operator|(WindowStyleFlags a, WindowStyleFlags b) noexcept
    {
        return (WindowStyleFlags)((uint32_t)a | (uint32_t)b);
    }

    constexpr bool HasFlag(WindowStyleFlags a, WindowStyleFlags b) noexcept
    {
        return (((uint32_t)a & (uint32_t)b) != 0);
    }

    // Apply styles without changing size/position.
    bool ApplyWindowStyles_NoResize(HWND hWnd, WindowStyleFlags flags) noexcept;
}
