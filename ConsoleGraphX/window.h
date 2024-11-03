#pragma once
#include <string>
#include <windows.h>
#include <errhandlingapi.h>
#include <iostream>
#include <processthreadsapi.h>
#include <filesystem>
#include <handleapi.h>
#include <vector>
#include "../WinCore/WinCore.h"
#include "events.h"
#include "shared_window_memory.h"
#include "pixel_buffer_shared.h"
#include "screen.h"


namespace ConsoleGraphX
{
    enum class WindowType
    {
        UserCreated,
        EngineCreated
    };

    
    struct WindowPositionData
    {
        // uint16 is used here as unsinged short does not guarantee 16 bit

        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height;
    };

    class Window : public ConsoleGraphX_Internal::Screen
    {
    public:
        CGXEventArgs<unsigned short, unsigned short> OnWindowCreated;
        CGXEventArgs<unsigned short, unsigned short> OnWindowResized;

        CGXEventArgs<unsigned int> OnKeyPressed;
        CGXEventArgs<unsigned int> OnKeyReleased;

        CGXEventArgs<Window*> OnWindowDestroyed;


        CGXEvent OnWindowFocusGained;
        CGXEvent OnWindowFocusLost;

        CGXEvent OnRenderFrame;

    private:
        HANDLE _m_hMapFile;
        HANDLE _m_processHandle;
        std::string _m_windowName;
        SharedWindowMemory* _m_sharedMem = nullptr;

    protected:
        HWND _m_windowHWND;


    public:
        Window(unsigned short width, unsigned short height, const std::string& windowName, std::unique_ptr<ConsoleGraphX_Internal::PixelBuffer> sBuffer, unsigned short fontWidth = 3, unsigned short fontHeight = 3)
            : Screen(width, height, fontWidth, fontHeight, std::move(sBuffer)), 
            _m_windowName(windowName), _m_processHandle(INVALID_HANDLE_VALUE), _m_hMapFile(INVALID_HANDLE_VALUE), _m_windowHWND(NULL)
        {}

        Window(unsigned short width, unsigned short height, const std::string& windowName, unsigned short fontWidth = 3, unsigned short fontHeight = 3)
            : Screen(width, height, fontWidth, fontHeight), 
            _m_windowName(windowName), _m_processHandle(INVALID_HANDLE_VALUE), _m_hMapFile(INVALID_HANDLE_VALUE), _m_windowHWND(NULL)
        {
            SetConsoleTitleA(windowName.c_str());
        }

        void Destroy()
        {
            // We use a separate Destroy method instead of the destructor because we want to control 
            // when the cleanup happens, especially in cases where the WindowManager is handling 
            // multiple windows. By explicitly calling Destroy, we can ensure resources are released 
            // in a specific order and that any registered event listeners are notified before the 
            // object is actually deleted. Invoking OnWindowDestroyed here allows other systems to 
            // respond to the destruction of this window in a managed and predictable way, which 
            // wouldn't be possible if we relied solely on the destructor.
            TerminateProcess(_m_processHandle, 0);
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


        void CreateConsoleWindow()
        {
            _CreateWindowImpl(_m_width, _m_height, _m_pixelWidth, _m_pixelHeight, _m_windowName);
            _AccessSharedMemory();

            _m_windowHWND = FindWindowA(NULL, _m_windowName.c_str());

            _m_screenBuffer = std::make_unique<ConsoleGraphX_Internal::PixelBuffer>(_m_width, _m_height, _m_sharedMem->m_buffer);
        }

        virtual WindowType GetType() const
        {
            return WindowType::UserCreated;
        }

        std::string_view GetWindowName() const
        {
            return _m_windowName;
        }

        void SetWindowPosition(unsigned short x, unsigned short y)
        {
            WindowPositionData wp = GetWindowPosition();

            SetWindowPos(_m_windowHWND, NULL, x, y, wp.width, wp.height, SWP_NOZORDER | SWP_SHOWWINDOW);
        }

        WindowPositionData GetWindowPosition()
        {
            if (_m_windowHWND == NULL) 
            {
                _TrySetWindowHandle();
            }

            if (_m_windowHWND == NULL)
                return { 0, 0, 0 ,0 };

            RECT sizeData{};
            if (!GetWindowRect(_m_windowHWND, &sizeData)) 
            {
                std::cerr << "Failed to get window rect: " << GetLastError() << std::endl;
                return { 0, 0, 0, 0 }; 
            }

            WindowPositionData positionData{};
            positionData.x = static_cast<uint16_t>(sizeData.left);
            positionData.y = static_cast<uint16_t>(sizeData.top);
            positionData.width = static_cast<uint16_t>(sizeData.right - sizeData.left);
            positionData.height = static_cast<uint16_t>(sizeData.bottom - sizeData.top);

            return positionData;
        }

        std::string& GetWindowNameR() 
        {
            return _m_windowName;
        }

        void ResizeWindow(unsigned short newWidth, unsigned short newHeight)
        {
            OnWindowResized.InvokeNF(newWidth, newHeight); 
        }

        void RenderFrame()
        {
            OnRenderFrame.Invoke(); 
        }

    protected:
        virtual void _CreateWindowImpl(short width, short height, short fontWidth, short fontHeight, const std::string& windowName)
        {
            std::string exePath =  "WindowHandler.exe";
            
            bool isEngineWindow = GetType() == WindowType::EngineCreated;
            
            std::vector args = {
               std::to_string(width),
               std::to_string(height),
               std::to_string(fontWidth),
               std::to_string(fontHeight),
               std::to_string(isEngineWindow),
               windowName
            };

            _m_processHandle = CreateProcessWC(exePath, args);
        }

        void _AccessSharedMemory()
        {
            size_t sharedMemorySize = sizeof(SharedWindowMemory) + sizeof(CHAR_INFO) * _m_width * _m_height;

            _m_hMapFile = AccessSharedMemoryWC(_m_windowName, sharedMemorySize);

            _m_sharedMem = static_cast<SharedWindowMemory*>(MapViewOfFile(_m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sharedMemorySize));
        }

        void _TrySetWindowHandle()
        {
            HWND hwnd = FindWindowA(NULL, _m_windowName.c_str());

            if (hwnd == NULL)
            {
                int error = GetLastError();

                std::cerr << "Failed to find window: " << error << std::endl;

                return;
            }

            _m_windowHWND = hwnd;
        }
    };

    class EmbeddedWindow : public Window
    {
    public:
        EmbeddedWindow(unsigned short width, unsigned short height, const std::string& windowName, unsigned short fontWidth = 3, unsigned short fontHeight = 3):
            Window(width, height, windowName, fontWidth, fontHeight)
        {}

    protected:
        virtual void _CreateWindowImpl(short width, short height, short fontWidth, short fontHeight, const std::string& windowName) override
        {
            _m_windowHWND = GetConsoleWindow();
        }
    };
};
