#include "PCH_CGX.h"
#include <processthreadsapi.h>
#include <filesystem>
#include <handleapi.h>
#include <exception>
#include "window.h"
#include "../WinCore/WinCore.h"
#include "../WinCore/console_handler.h"

namespace ConsoleGraphX
{
    Window::Window(unsigned short width, unsigned short height, unsigned short fontWidth, unsigned short fontHeight, const std::string& windowName)
        : AbstractWindow(windowName), Screen(width, height, fontWidth, fontHeight)
    {
        SetConsoleTitleA(windowName.c_str());
        _m_windowHWND = GetConsoleWindow();
    }

    void Window::Destroy()
    {
        OnWindowDestroyed.Invoke(this);
    }

    void Window::SetupWindow()
    {
        std::string closeEventName = WINDOW_CLOSE_EVENT_NAME(std::string(_m_windowName));

        HANDLE closeEvent = CreateEventA(nullptr, TRUE, FALSE, closeEventName.c_str());
        if (closeEvent == INVALID_HANDLE_VALUE)
        {
            DWORD errorCode = GetLastError();
            std::cerr << "Failed to create close event. Error: " << errorCode << std::endl;
            throw std::runtime_error("Fatal: failed to make window close event");
        }

        std::function<void()> windowCloseCallback = [this, closeEvent]()
            {
                SetEvent(closeEvent);
            };

        WinCore::ConsoleHandler::RegisterCloseCallback(windowCloseCallback);
        WinCore::ConsoleHandler::SetHandler();

        _m_closeEvent = closeEvent;
    }

    CrossProcessWindow::CrossProcessWindow(unsigned short width, unsigned short height, unsigned short fontWidth, unsigned short fontHeight,
        const std::string& windowName, std::unique_ptr<ConsoleGraphX_Internal::PixelBuffer> sBuffer)
        : AbstractWindow(windowName), Screen(width, height, fontWidth, fontHeight, std::move(sBuffer)),
        _m_processHandle(INVALID_HANDLE_VALUE), _m_hMapFile(INVALID_HANDLE_VALUE), _m_sharedMem(nullptr)
    {}

    void CrossProcessWindow::Destroy()
    {
        bool terminationStatus = TerminateProcess(_m_processHandle, 0);
        CloseHandle(_m_processHandle);

        if (_m_sharedMem)
        {
            UnmapViewOfFile(_m_sharedMem);
        }

        if (_m_hMapFile)
        {
            CloseHandle(_m_hMapFile);
        }

        OnWindowDestroyed.Invoke(this);
    }

    void CrossProcessWindow::SetupWindow()
    {
        _CreateWindowImpl(_m_width, _m_height, _m_pixelWidth, _m_pixelHeight, _m_windowName);
        _AccessSharedMemory();

        _m_windowHWND = FindWindowA(NULL, _m_windowName.c_str());

        _m_screenBuffer = std::make_unique<ConsoleGraphX_Internal::PixelBuffer>(_m_width, _m_height, _m_sharedMem->m_buffer);
    }

    void CrossProcessWindow::_CreateWindowImpl(short width, short height, short fontWidth, short fontHeight, const std::string& windowName)
    {
        std::string exePath = "WindowHandler.exe";
        std::vector args = {
            std::to_string(width),
            std::to_string(height),
            std::to_string(fontWidth),
            std::to_string(fontHeight),
            windowName
        };

        _m_processHandle = WinCore::CGXCreateProcess(exePath, args);
    }

    void CrossProcessWindow::_AccessSharedMemory()
    {
        size_t sharedMemorySize = sizeof(SharedWindowMemory) + sizeof(CHAR_INFO) * _m_width * _m_height;

        _m_hMapFile = WinCore::AccessSharedMemory(_m_windowName, sharedMemorySize);

        _m_sharedMem = static_cast<SharedWindowMemory*>(MapViewOfFile(_m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sharedMemorySize));
    }
}
