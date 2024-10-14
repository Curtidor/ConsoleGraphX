#pragma once
#include "events.h"

namespace ConsoleGraphX
{
    class Window
    {
    public:
        CGXEventArgs<short, short> OnWindowCreated;
        CGXEventArgs<short, short> OnWindowResized;

        CGXEventArgs<int> OnKeyPressed;
        CGXEventArgs<int> OnKeyReleased;

        CGXEvent OnWindowFocusGained;
        CGXEvent OnWindowFocusLost;

        CGXEvent OnRenderFrame;
        CGXEvent OnWindowDestroyed;


        void CreateWindow(short width, short height)
        {
            OnWindowCreated.InvokeNF(width, height); 
        }

        void ResizeWindow(short newWidth, short newHeight)
        {
            OnWindowResized.InvokeNF(newWidth, newHeight); 
        }

        void RenderFrame()
        {
            // Rendering logic...
            OnRenderFrame.Invoke(); // Trigger event
        }
    };
};
