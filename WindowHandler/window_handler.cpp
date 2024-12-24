#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <windows.h>
#include <chrono>
#include <string>
#include <synchapi.h>
#include <consoleapi2.h>
#include <handleapi.h>
#include <processenv.h>
#include <wincontypes.h>
#include <errhandlingapi.h>
#include <memoryapi.h>
#include "../WinCore/WinCore.h"
#include "../WinCore/console_handler.h"
#include "../ConsoleGraphX/window_styles.h"
#include "../ConsoleGraphX/shared_window_memory.h"
#include "../ConsoleGraphX/screen.h"
#include "../ConsoleGraphX/pixel_buffer.h"
#include "../ConsoleGraphX/abstract_window.h"
#include "console_handler.h"

/**
 * @brief Initializes the console settings for the application.
 *
 * @param hConsole Handle to the console.
 * @param screenWidth Width of the screen buffer in characters.
 * @param screenHeight Height of the screen buffer in characters.
 * @param fontWidth Width of the font in pixels.
 * @param fontHeight Height of the font in pixels.
 * @param appName Title of the application.
 */
static void InitializeConsole(HANDLE hConsole, unsigned short screenWidth, unsigned short screenHeight,
    unsigned short fontWidth, unsigned short fontHeight, const char* appName) 
{
    WinCore::SetConsoleWindowSize(hConsole, 1, 1);
    SetConsoleScreenBufferSize(hConsole, { static_cast<short>(screenWidth), static_cast<short>(screenHeight) });

    // get maximum screen size and set the window size
    CONSOLE_SCREEN_BUFFER_INFO cInfo{};
    GetConsoleScreenBufferInfo(hConsole, &cInfo);
    WinCore::SetConsoleWindowSize(hConsole, cInfo.dwSize.X - 1, cInfo.dwSize.Y - 1);

    WinCore::SetConsoleFontSize(hConsole, fontWidth, fontHeight);
    SetConsoleTitleA(appName);
    WinCore::DisableConsoleResize();

    // TEMP (need a better way to excluded the editor) apply borderless style for non-editor applications
    if (strcmp(appName, "Editor") != 0) 
    {
        ApplyWindowStyles(WindowStyles::Borderless, GetConsoleWindow());
    }
}

static HANDLE InitializeSharedMemory(const char* windowName, DWORD sharedMemorySize, SharedWindowMemory*& sharedMem, unsigned short screenWidth, unsigned short screenHeight)
{
    HANDLE hMapFile = WinCore::CreateSharedMemory(sharedMemorySize, windowName);
    if (!hMapFile) return nullptr;

    sharedMem = static_cast<SharedWindowMemory*>(WinCore::MapSharedMemory(hMapFile, sharedMemorySize));
    if (!sharedMem) {
        CloseHandle(hMapFile);
        return nullptr;
    }

    new (sharedMem) SharedWindowMemory(screenWidth * screenHeight);
    return hMapFile;
}


/**
 * @brief Main rendering loop to manage screen updates and FPS display.
 *
 * @param screen Reference to the Screen object.
 * @param closeEvent event that signals console termination
 */
static void RunMainLoop(ConsoleGraphX_Internal::Screen& screen, HANDLE closeEvent)
{
    int frameCount = 0;
    float fpsTimeCounter = 0.0f;
    auto startTime = std::chrono::high_resolution_clock::now();

    while (true) 
    {
        if (WaitForSingleObject(closeEvent, 0) == WAIT_OBJECT_0)
        {
            std::cout << "Close event triggered. Exiting main loop..." << std::endl;
            break;
        }

        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - startTime;
        fpsTimeCounter += deltaTime.count();
        startTime = currentTime;

        if (fpsTimeCounter >= 1.0f) 
        {
            frameCount = 0;
            fpsTimeCounter = 0.0f;
        }

        if (!screen.DrawScreen()) 
        {
            std::cerr << "Failed to write to console: " << GetLastError() << std::endl;
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 6) // 6 because the command to create this process is >WindowHandler.exe 100 100 6 6 "Test"
    {
        std::cerr << "Usage: <screenWidth> <screenHeight> <fontWidth> <fontHeight> <appName>" << std::endl;
        return 1;
    }

    unsigned short screenWidth = static_cast<unsigned short>(std::stoi(argv[1]));
    unsigned short screenHeight = static_cast<unsigned short>(std::stoi(argv[2]));
    unsigned short fontWidth = static_cast<unsigned short>(std::stoi(argv[3]));
    unsigned short fontHeight = static_cast<unsigned short>(std::stoi(argv[4]));
    const char* windowName = argv[5];

    // create a named event for signaling on console close
    std::string closeEventName = WINDOW_CLOSE_EVENT_NAME(std::string(windowName));
    WinCore::ScopedHandle closeEvent(CreateEventA(nullptr, TRUE, FALSE, closeEventName.c_str()));
    if (!closeEvent.get())
    {
        DWORD errorCode = GetLastError();
        std::cerr << "Failed to create close event. Error: " << errorCode << std::endl;
        return 1;
    }

    // create a cleanup event
    WinCore::ScopedHandle cleanupEvent(CreateEventA(nullptr, TRUE, FALSE, nullptr));
    if (!cleanupEvent.get())
    {
        DWORD errorCode = GetLastError();
        std::cerr << "Failed to create cleanup event. Error: " << errorCode << std::endl;
        return 1;
    }

    // register a callback to signal the close event and wait for cleanup
    auto closeCallback = [closeEvent = closeEvent.get(), cleanupEvent = cleanupEvent.get()]() 
        {
            SetEvent(closeEvent); // signal close event
            WaitForSingleObject(cleanupEvent, INFINITE); // wait until cleanup is done
        };

    WinCore::ConsoleHandler::RegisterCloseCallback(closeCallback);
    WinCore::ConsoleHandler::SetHandler();

    // initialize shared memory
    SharedWindowMemory* sharedMem = nullptr;
    DWORD charInfoSize = sizeof(CHAR_INFO) * screenWidth * screenHeight;
    DWORD sharedMemorySize = sizeof(SharedWindowMemory) + charInfoSize;

    WinCore::ScopedHandle hMapFile(InitializeSharedMemory(windowName, sharedMemorySize, sharedMem, screenWidth, screenHeight));
    if (!hMapFile.get())
    {
        return 1;
    }

    WinCore::ScopedHandle hConsole(GetStdHandle(STD_OUTPUT_HANDLE));
    if (hConsole.get() == INVALID_HANDLE_VALUE)
    {
        UnmapViewOfFile(sharedMem);
        return 1;
    }

    // prepare screen buffer
    CHAR_INFO* sharedBuffer = reinterpret_cast<CHAR_INFO*>(sharedMem + 1);
    std::memset(sharedMem->m_buffer, 0, sharedMem->m_bufferSize * sizeof(CHAR_INFO));

    auto sharedScreenBuffer = std::make_unique<ConsoleGraphX_Internal::PixelBufferHandle>(
        ConsoleGraphX_Internal::PixelBufferHandle(hConsole.get(), screenWidth, screenHeight, sharedBuffer));

    ConsoleGraphX_Internal::Screen screen(screenWidth, screenHeight, fontWidth, fontHeight, std::move(sharedScreenBuffer));

    InitializeConsole(hConsole.get(), screenWidth, screenHeight, fontWidth, fontHeight, windowName);
    RunMainLoop(screen, closeEvent.get());

    // cleanup
    UnmapViewOfFile(sharedMem);
    SetEvent(cleanupEvent.get()); // signal that cleanup is complete

    return 0;
}


