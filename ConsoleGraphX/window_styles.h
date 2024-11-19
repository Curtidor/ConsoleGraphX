#pragma once
#include <windows.h>

// Enum class for window styles
enum class WindowStyles : unsigned int
{
    None = 0,
    NoResize = 1 << 0, // Removes the resize border (WS_SIZEBOX)
    NoMaximize = 1 << 1, // Removes the maximize button (WS_MAXIMIZEBOX)
    NoMinimize = 1 << 2, // Removes the minimize button (WS_MINIMIZEBOX)
    Borderless = 1 << 3, // Removes the window border (WS_CAPTION | WS_THICKFRAME)
    NoTitleBar = 1 << 4, // Removes the title bar and system menu (WS_CAPTION | WS_SYSMENU)
    NoSystemMenu = 1 << 5  // Removes the system menu (WS_SYSMENU)
};

// Bitwise operators for WindowStyles
WindowStyles operator|(WindowStyles a, WindowStyles b);
WindowStyles& operator|=(WindowStyles& a, WindowStyles b);
bool operator&(WindowStyles a, WindowStyles b);

// Function to apply the selected window styles
void ApplyWindowStyles(WindowStyles styles, HWND hWnd);
