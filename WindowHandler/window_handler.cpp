#include <iostream>
#include <Windows.h>
#include <string>
#include <synchapi.h>
#include <consoleapi2.h>
#include <handleapi.h>
#include <processenv.h>
#include <wincontypes.h>
#include <errhandlingapi.h>
#include <memoryapi.h>
#include "shared_memory.h"
#include "../ConsoleGraphX/screen.h"
#include "../ConsoleGraphX/screen_buffer_shared.h"


static HANDLE CreateSharedMemory(size_t totalSize, const char* name)
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

    short screenWidth = static_cast<short>(std::stoi(argv[1]));
    short screenHeight = static_cast<short>(std::stoi(argv[2]));
    short fontWidth = static_cast<short>(std::stoi(argv[3]));
    short fontHeight = static_cast<short>(std::stoi(argv[4]));
    const char* appName = argv[5];

    // calculate the total size required for the shared memory
    size_t charInfoSize = sizeof(CHAR_INFO) * screenWidth * screenHeight;
    size_t sharedMemorySize = sizeof(SharedMemory) + charInfoSize;

    const char* sharedMemoryName = "SharedMemoryInput";

    HANDLE hMapFile = CreateSharedMemory(sharedMemorySize, sharedMemoryName);
    if (!hMapFile)
    {
        std::cerr << "Failed to create or open shared memory." << std::endl;
        return 1;
    }

    SharedMemory* sharedMem = static_cast<SharedMemory*>(MapSharedMemory(hMapFile, sharedMemorySize));
    if (!sharedMem)
    {
        std::cerr << "Failed to map shared memory." << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }

    // initialize the shared memory structure
    new (sharedMem) SharedMemory(screenWidth * screenHeight);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to get handle to the console output." << std::endl;
        UnmapViewOfFile(sharedMem);
        CloseHandle(hMapFile);
        return 1;
    }

    // the buffer should be right after the SharedMemory structure
    CHAR_INFO* sharedBuffer = reinterpret_cast<CHAR_INFO*>(sharedMem + 1);

    ConsoleGraphX_Internal::ScreenBufferShared sharedScreenBuffer(hConsole, sharedBuffer, screenWidth, screenHeight);

    ConsoleGraphX_Internal::Screen screen(screenWidth, screenHeight, fontWidth, fontHeight, &sharedScreenBuffer);

    SetConsoleTitleA(appName);

    while (true)
    {
        if (!screen.DrawScreen())
        {
            std::cerr << "Failed to write to console: " << GetLastError() << std::endl;
            break;
        }

        Sleep(1);

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            std::cout << "ESC pressed. Exiting..." << std::endl;
            break;
        }
    }

    // cleanup
    UnmapViewOfFile(sharedMem);
    CloseHandle(hMapFile);

    return 0;
}

