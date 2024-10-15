#pragma once
#include "window.h"

namespace ConsoleGraphX_Internal
{
    std::unique_ptr<ConsoleGraphX::Window> CreateUserWindow(short width, short height, const std::string& windowName, short fontWidth = 3, short fontHeight = 3);
    std::unique_ptr<ConsoleGraphX::Window> CreateEngineWindow(short width, short height, const std::string& windowName, short fontWidth = 3, short fontHeight = 3);
};
