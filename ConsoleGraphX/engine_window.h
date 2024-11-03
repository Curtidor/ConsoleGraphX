#pragma once
#include "window.h"

namespace ConsoleGraphX
{
    class EngineWindow : public Window
    {
    public:
        EngineWindow(short width, short height, const std::string& windowName, short fontWidth = 3, short fontHeight = 3)
            : Window(width, height, windowName, nullptr, fontWidth, fontHeight)
        {}

        WindowType GetType() const override
        {
            return WindowType::EngineCreated;
        }

    };
};
