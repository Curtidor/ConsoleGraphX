#pragma once
#include <string>
#include <windows.h>
#include <errhandlingapi.h>
#include <iostream>
#include <processthreadsapi.h>
#include <filesystem>
#include <handleapi.h>
#include "events.h"
#include <vector>
#include "shared_window_memory.h"
#include "logger.h"


namespace ConsoleGraphX
{
    enum class WindowType
    {
        UserCreated,
        EngineCreated
    };

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

    public:

        Window(short width, short height, const std::string& windowName, short fontWidth = 3, short fontHeight = 3)
            : _m_width(width), _m_height(height), _m_windowName(windowName), _m_logger("WindowLogger")
        {
            //_CreateWindowImpl(width, height, fontWidth, fontHeight, windowName);
            _AccessSharedMemory();
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

        void ResizeWindow(short newWidth, short newHeight)
        {
            OnWindowResized.InvokeNF(newWidth, newHeight); 
        }

        void RenderFrame()
        {
            OnRenderFrame.Invoke(); 
        }

        void Test(unsigned short color)
        {
            std::fill(_m_sharedMem->m_buffer, _m_sharedMem->m_buffer + _m_sharedMem->m_bufferSize, CHAR_INFO{ '#', color });
        }

    private:
        void _CreateWindowImpl(short width, short height, short fontWidth, short fontHeight, const std::string& windowName)
        {
            std::filesystem::path exePath;

        #ifdef _DEBUG
            exePath = std::filesystem::path("../x64/Debug/WindowHandler.exe");
        #else
            exePath = std::filesystem::path("../x64/Release/WindowHandler.exe");
        #endif

            std::string commandLine = exePath.string() + " " +
                std::to_string(width) + " " +
                std::to_string(height) + " " +
                std::to_string(fontWidth) + " " +
                std::to_string(fontHeight) + " \"" +
                windowName.c_str();

            // create a writable buffer for CreateProcessA
            std::vector<char> commandLineBuffer(commandLine.begin(), commandLine.end());
            commandLineBuffer.push_back('\0'); 

            STARTUPINFOA si = { sizeof(STARTUPINFOA) };
            PROCESS_INFORMATION pi = {};

            if (!CreateProcessA(NULL, commandLineBuffer.data(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
            {
                std::cerr << "Failed to create WindowHandler process. Error: " << GetLastError() << std::endl;
                return;
            }

            _m_processHandle = pi.hProcess;
            CloseHandle(pi.hThread);
        }

        void _AccessSharedMemory()
        {
            _m_hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, "TEST");
            if (_m_hMapFile == NULL)
            {
                int error = GetLastError();
                _m_logger.LogMessage("Failed to open shared memory object: " + std::to_string(error));
                return;
            }

            size_t sharedMemorySize = sizeof(SharedWindowMemory) + sizeof(CHAR_INFO) * _m_width * _m_height;
            _m_logger.LogMessage("MEM SIZE" + std::to_string(sharedMemorySize));
            _m_sharedMem = static_cast<SharedWindowMemory*>(MapViewOfFile(_m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sharedMemorySize));
            if (_m_sharedMem == NULL)
            {
                int error = GetLastError();
                _m_logger.LogMessage("Failed to map shared memory: " + std::to_string(error));

                CloseHandle(_m_hMapFile);
                _m_hMapFile = NULL;
                return;
            }

            _m_logger.LogMessage("Successfully mapped shared memory.");
        }


    private:
        short _m_width;
        short _m_height;
        std::string _m_windowName;
        ConsoleGraphX_Internal::Logger _m_logger;
        HANDLE _m_processHandle;
        HANDLE _m_hMapFile = NULL;  
        SharedWindowMemory* _m_sharedMem = nullptr; 
    };

    class EngineWindow : public Window
    {
    public:
        EngineWindow(short width, short height, const std::string& windowName, short fontWidth = 3, short fontHeight = 3)
            : Window(width, height, windowName, fontWidth, fontHeight)
        {}

        WindowType GetType() const override
        {
            return WindowType::EngineCreated;
        }
        
    };


};
