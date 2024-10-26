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

    class Window : public ConsoleGraphX_Internal::Screen
    {
    public:
        CGXEventArgs<unsigned short, unsigned short> OnWindowCreated;
        CGXEventArgs<unsigned short, unsigned short> OnWindowResized;

        CGXEventArgs<unsigned int> OnKeyPressed;
        CGXEventArgs<unsigned int> OnKeyReleased;

        CGXEvent OnWindowFocusGained;
        CGXEvent OnWindowFocusLost;

        CGXEvent OnRenderFrame;
        CGXEvent OnWindowDestroyed;

    private:
        HANDLE _m_hMapFile;
        HANDLE _m_processHandle;
        std::string _m_windowName;
        SharedWindowMemory* _m_sharedMem = nullptr;


    public:
        Window(unsigned short width, unsigned short height, const std::string& windowName, unsigned short fontWidth = 3, unsigned short fontHeight = 3)
            : Screen(width, height, fontWidth, fontHeight, nullptr), //null ptr the buffer as we want to do the allocation
            _m_windowName(windowName)
        {
            _CreateWindowImpl(width, height, fontWidth, fontHeight, windowName);
            _AccessSharedMemory();

            _m_screenBuffer = std::make_unique<ConsoleGraphX_Internal::PixelBuffer>(width, height, _m_sharedMem->m_buffer);
            // Now that the buffer is set call initialize on the screen
            Initialize();
        }

        ~Window()
        {
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
        }

        virtual WindowType GetType() const
        {
            return WindowType::UserCreated;
        }

        std::string_view GetWindowName() const
        {
            return _m_windowName;
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

    private:
        void _CreateWindowImpl(short width, short height, short fontWidth, short fontHeight, const std::string& windowName)
        {
            std::string exePath =  GetBuildSpecificFilePath("WindowHandler.exe").u8string();

            std::vector args = {
               std::to_string(width),
               std::to_string(height),
               std::to_string(fontWidth),
               std::to_string(fontHeight),
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
    };
};
