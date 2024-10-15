#include "CGXPCH.h"
#include "window_factory.h"

std::unique_ptr<ConsoleGraphX::Window> ConsoleGraphX_Internal::CreateUserWindow(short width, short height, const std::string& windowName, short fontWidth, short fontHeight)
{
    return std::make_unique<ConsoleGraphX::Window>(width, height, windowName, fontWidth, fontHeight);
}

std::unique_ptr<ConsoleGraphX::Window> ConsoleGraphX_Internal::CreateEngineWindow(short width, short height, const std::string& windowName, short fontWidth, short fontHeight)
{
    return std::make_unique<ConsoleGraphX::EngineWindow>(width, height, windowName, fontWidth, fontHeight);
}
