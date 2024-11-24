#include <iostream>
#include <Windows.h>
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
#include "../ConsoleGraphX/window_styles.h"
#include "../ConsoleGraphX/shared_window_memory.h"
#include "../ConsoleGraphX/screen.h"
#include "../ConsoleGraphX/pixel_buffer.h"


static HANDLE CreateSharedMemory(DWORD totalSize, LPCSTR name)
{
    HANDLE hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,    // Use the paging file
        NULL,                    // Default security
        PAGE_READWRITE,          // Read/write access
        0,                       // Maximum object size (high-order DWORD)
        totalSize,               // Maximum object size (low-order DWORD)
        name);                   // Name of the mapping object

    if (hMapFile == NULL)
    {
        std::cerr << "Could not create file mapping object: " << GetLastError() << std::endl;
    }

    return hMapFile;
}

static void* MapSharedMemory(HANDLE hMapFile, size_t size)
{
    void* pBuf = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, size);
    if (pBuf == NULL)
    {
        std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
    }

    return pBuf;
}


int main(int argc, char* argv[])
{
    if (argc < 5)
    {
        std::cerr << "Usage: <screenWidth> <screenHeight> <fontWidth> <fontHeight> <appName>" << std::endl;
        return 1;
    }

    unsigned short screenWidth = static_cast<short>(std::stoi(argv[1]));
    unsigned short screenHeight = static_cast<short>(std::stoi(argv[2]));
    unsigned short fontWidth = static_cast<short>(std::stoi(argv[3]));
    unsigned short fontHeight = static_cast<short>(std::stoi(argv[4]));
    const char* appName = argv[5];

    DWORD charInfoSize = sizeof(CHAR_INFO) * screenWidth * screenHeight;
    DWORD sharedMemorySize = sizeof(SharedWindowMemory) + charInfoSize;

    HANDLE hMapFile = CreateSharedMemory(sharedMemorySize, appName);
    if (!hMapFile)
    {
        std::cerr << "Failed to create or open shared memory." << std::endl;
        return 1;
    }

    SharedWindowMemory* sharedMem = static_cast<SharedWindowMemory*>(MapSharedMemory(hMapFile, sharedMemorySize));
    if (!sharedMem)
    {
        std::cerr << "Failed to map shared memory." << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }

    new (sharedMem) SharedWindowMemory(screenWidth * screenHeight);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE)
    {
        UnmapViewOfFile(sharedMem);
        CloseHandle(hMapFile);
        return 1;
    }

    CHAR_INFO* sharedBuffer = reinterpret_cast<CHAR_INFO*>(sharedMem + 1);
    std::memset(sharedMem->m_buffer, 0, sharedMem->m_bufferSize * sizeof(CHAR_INFO));

    std::unique_ptr<ConsoleGraphX_Internal::PixelBufferHandle> sharedScreenBuffer = std::make_unique< ConsoleGraphX_Internal::PixelBufferHandle>(ConsoleGraphX_Internal::PixelBufferHandle(hConsole, screenWidth, screenHeight, sharedBuffer));

    ConsoleGraphX_Internal::Screen screen(screenWidth, screenHeight, fontWidth, fontHeight, std::move(sharedScreenBuffer));

    WinCore::SetConsoleWindowSize(hConsole, 1, 1);
    SetConsoleScreenBufferSize(hConsole, { static_cast<short>(screenWidth),  static_cast<short>(screenHeight) });

    // get max screen size
    CONSOLE_SCREEN_BUFFER_INFO cInfo{};
    GetConsoleScreenBufferInfo(hConsole, &cInfo);

    WinCore::SetConsoleWindowSize(hConsole, cInfo.dwSize.X - 1, cInfo.dwSize.Y - 1);

    WinCore::SetConsoleFontSize(hConsole, fontWidth, fontHeight);


    SetConsoleTitleA(appName);
    WinCore::DisableConsoleResize();
    
    //TEMP CODE
    if (strcmp(appName, "Editor") != 0)
    {
        ApplyWindowStyles(WindowStyles::Borderless, GetConsoleWindow());
    }



    // FPS counter variables
    int frameCount = 0;
    float fpsTimeCounter = 0.0f;
    auto startTime = std::chrono::high_resolution_clock::now();

    while (true)
    {
        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - startTime;
        fpsTimeCounter += deltaTime.count();
        startTime = currentTime;

        // Every second, update the console title with the FPS
        if (fpsTimeCounter >= 1.0f)
        {
            //std::string newTitle = std::string(appName) + " - FPS: " + std::to_string(frameCount);
            //SetConsoleTitleA(newTitle.c_str());

            frameCount = 0;
            fpsTimeCounter = 0.0f;
        }

        if (!screen.DrawScreen())
        {
            std::cerr << "Failed to write to console: " << GetLastError() << std::endl;
            break;
        }
    }

    // Cleanup
    UnmapViewOfFile(sharedMem);
    CloseHandle(hMapFile);

    return 0;
}


