#include "PCH_CGX.h"
#include "Engine\Core\Window\window_styles.h"

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
    Sleep(1000); // TODO, why does sleep fix everything window related... not good need to find a correct fix
    // get the current window size
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;

    // get the current window style
    LONG style = GetWindowLong(hWnd, GWL_STYLE);

    // apply the new styles
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

    SetWindowLong(hWnd, GWL_STYLE, style);

    RECT adjustedRect = { 0, 0, clientWidth, clientHeight };
    bool adjustOk = AdjustWindowRect(&adjustedRect, style, FALSE); // adjust for the new window style

    int adjustedWidth = adjustedRect.right - adjustedRect.left;
    int adjustedHeight = adjustedRect.bottom - adjustedRect.top;

    SetWindowPos(hWnd, nullptr,
        0, 0,
        adjustedWidth, adjustedHeight,
        SWP_NOZORDER | SWP_NOMOVE | SWP_FRAMECHANGED | SWP_NOACTIVATE);
}
