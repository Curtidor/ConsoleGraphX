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
    // get the current window style
    LONG style = GetWindowLong(hWnd, GWL_STYLE);

    if (styles & WindowStyles::NoResize)
        style &= ~WS_SIZEBOX;

    if (styles & WindowStyles::NoMaximize)
        style &= ~WS_MAXIMIZEBOX;

    if (styles & WindowStyles::NoMinimize)
        style &= ~WS_MINIMIZEBOX;

    if (styles & WindowStyles::Borderless)
        style &= ~(WS_CAPTION | WS_THICKFRAME);

    if (styles & WindowStyles::NoTitleBar)
        style &= ~(WS_CAPTION | WS_SYSMENU);

    if (styles & WindowStyles::NoSystemMenu)
        style &= ~WS_SYSMENU;

    // set the new window style
    SetWindowLong(hWnd, GWL_STYLE, style);

    // apply the style changes
    SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE);
}
