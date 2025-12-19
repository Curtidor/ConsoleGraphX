#pragma once
#include <windows.h>
#include "Engine\Math\vector2.h"

enum class WindowStyles : unsigned int
{
    None = 0,
    NoResize = 1 << 0, // removes the resize border (WS_SIZEBOX)
    NoMaximize = 1 << 1, // removes the maximize button (WS_MAXIMIZEBOX)
    NoMinimize = 1 << 2, // removes the minimize button (WS_MINIMIZEBOX)
    Borderless = 1 << 3, // removes the window border (WS_CAPTION | WS_THICKFRAME)
    NoTitleBar = 1 << 4, // removes the title bar and system menu (WS_CAPTION | WS_SYSMENU)
    NoSystemMenu = 1 << 5  // removes the system menu (WS_SYSMENU)
};

// bitwise operators for WindowStyles
WindowStyles operator|(WindowStyles a, WindowStyles b);
WindowStyles& operator|=(WindowStyles& a, WindowStyles b);
bool operator&(WindowStyles a, WindowStyles b);

// function to apply the selected window styles
void ApplyWindowStyles(WindowStyles styles, HWND hWnd, ConsoleGraphX::Vector2 windowSize);
