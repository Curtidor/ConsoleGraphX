#include "PCH_CGX.h"
#include "window_styles.h"

WindowStyles operator|(WindowStyles a, WindowStyles b)
{
    return static_cast<WindowStyles>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

WindowStyles& operator|=(WindowStyles& a, WindowStyles b)
{
    a = a | b;
    return a;
}

bool operator&(WindowStyles a, WindowStyles b)
{
    return static_cast<unsigned int>(a) & static_cast<unsigned int>(b);
}

void ApplyWindowStyles(WindowStyles styles, HWND hWnd)
{
    // Step 1: Get the current client area size
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;

    // Step 2: Get the current window style
    LONG style = GetWindowLong(hWnd, GWL_STYLE);

    // Step 3: Apply the new styles
    if (styles & WindowStyles::NoResize)
        style &= ~WS_SIZEBOX;

    if (styles & WindowStyles::NoMaximize)
        style &= ~WS_MAXIMIZEBOX;

    if (styles & WindowStyles::NoMinimize)
        style &= ~WS_MINIMIZEBOX;

    if (styles & WindowStyles::Borderless)
        style &= ~(WS_CAPTION | WS_OVERLAPPEDWINDOW); 

    if (styles & WindowStyles::NoTitleBar)
        style &= ~(WS_CAPTION | WS_SYSMENU);

    if (styles & WindowStyles::NoSystemMenu)
        style &= ~WS_SYSMENU;


    style &= ~WS_EX_NOACTIVATE;

    // Step 4: Set the new window style
    SetWindowLong(hWnd, GWL_STYLE, style);

    // Step 5: Calculate the new window size to maintain the current client area size
    RECT adjustedRect = { 0, 0, clientWidth, clientHeight };
    AdjustWindowRect(&adjustedRect, style, FALSE); // Adjust for the new window style

    int adjustedWidth = adjustedRect.right - adjustedRect.left;
    int adjustedHeight = adjustedRect.bottom - adjustedRect.top;

    // Step 6: Resize the window to the new size
    SetWindowPos(hWnd, nullptr,
        0, 0,
        adjustedWidth, adjustedHeight,
        SWP_NOZORDER | SWP_NOMOVE | SWP_FRAMECHANGED | SWP_NOACTIVATE);
}
