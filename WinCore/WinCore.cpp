#include "PCH_WCore.h"
#include "WinCore.h"

// SCREEN/CONSOLE HElPERS

bool SetConsoleFontSizeWC(HANDLE handle, short width, short height)
{
	// Set the console font size
	CONSOLE_FONT_INFOEX font = { sizeof(CONSOLE_FONT_INFOEX) };
	GetCurrentConsoleFontEx(handle, FALSE, &font);

	font.dwFontSize.X = width;
	font.dwFontSize.Y = height;
	if (!SetCurrentConsoleFontEx(handle, FALSE, &font)) 
	{
		return false;
	}

	return true;
}

void SetCursorPositionWC(HANDLE handle, short x, short y)
{
	SetConsoleCursorPosition(handle, COORD{ x, y });
}

//Sets the title of the console in the running process
void SetConsoleNameWC(const std::string& name)
{
	SetConsoleTitleA(name.c_str());
}

void SetConsoleWindowSizeWC(HANDLE handle, short width, short height)
{
	SMALL_RECT rect = { 0, 0, width - 1, height - 1 };
	SetConsoleWindowInfo(handle, TRUE, &rect);
}


// IPC HELPERS

HANDLE CreateProcessWC(const std::string& exePath, const std::vector<std::string>& args)
{
    // build the command line
    std::string commandLine = exePath;
    for (const auto& arg : args)
    {
        commandLine += " " + arg;
    }

    // create a writable buffer for CreateProcessA
    std::vector<char> commandLineBuffer(commandLine.begin(), commandLine.end());
    commandLineBuffer.push_back('\0');

    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION pi = {};

    if (!CreateProcessA(NULL, commandLineBuffer.data(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
    {
        std::cerr << "Failed to create process: " << GetLastError() << std::endl;
        return NULL;
    }

    CloseHandle(pi.hThread); // close thread handle since we only need the process handle
    return pi.hProcess;
}

HANDLE AccessSharedMemoryWC(const std::string& memoryName, size_t expectedSize, int retries, int retryDelayMs)
{
    HANDLE hMapFile = NULL;

    while (retries > 0)
    {
        hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, memoryName.c_str());
        if (hMapFile != NULL)
        {
            break; // successfully opened the file mapping
        }

        retries--;
        Sleep(retryDelayMs);
    }

    if (hMapFile == NULL)
    {
        std::cerr << "Failed to open shared memory object after retries: " << GetLastError() << std::endl;
        return NULL;
    }

    return hMapFile;
}

void* MapSharedMemoryWC(HANDLE hMapFile, size_t size)
{
    void* pSharedMem = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, size);
    if (pSharedMem == NULL)
    {
        std::cerr << "Failed to map shared memory: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        return NULL;
    }

    return pSharedMem;
}
