#include "PCH_WCore.h"
#include "WinCore.h"


// SCREEN/CONSOLE HElPERS

namespace WinCore
{
    bool SetConsoleFontSize(HANDLE handle, short width, short height)
    {
        CONSOLE_FONT_INFOEX font = { sizeof(CONSOLE_FONT_INFOEX) };
        GetCurrentConsoleFontEx(handle, FALSE, &font);

        font.dwFontSize.X = width;
        font.dwFontSize.Y = height;

        return SetCurrentConsoleFontEx(handle, FALSE, &font);
    }

    void SetCursorPosition(HANDLE handle, short x, short y)
    {
        SetConsoleCursorPosition(handle, COORD{ x, y });
    }

    //Sets the title of the console in the running process
    void SetConsoleName(const std::string& name)
    {
        SetConsoleTitleA(name.c_str());
    }

    int SetConsoleWindowSize(HANDLE handle, short width, short height)
    {
        SMALL_RECT rect = { 0, 0, width, height };
        bool consoleWasSet = SetConsoleWindowInfo(handle, TRUE, &rect);

        if (!consoleWasSet)
            return  GetLastError();
        else
            return 0;
    }

    // Disables resizing of the console in calling process
    void DisableConsoleResize()
    {
        HWND consoleWindow = GetConsoleWindow();

        // Remove the resize border and maximize button from the console window
        LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
        style &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX);
        SetWindowLong(consoleWindow, GWL_STYLE, style);
    }

    // Removes the console title bar in calling process
    void RemoveConsoleTitleBar()
    {
        HWND consoleWindow = GetConsoleWindow();
        if (consoleWindow)
        {
            LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
            style &= ~(WS_CAPTION | WS_THICKFRAME); // Removes title bar and resizable border
            style &= ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU); // Removes minimize/maximize/close buttons
            SetWindowLong(consoleWindow, GWL_STYLE, style);

            // Apply the new style
            SetWindowPos(consoleWindow, NULL, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }

    // IPC HELPERS

    HANDLE CGXCreateProcess(const std::string& exePath, const std::vector<std::string>& args)
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
            std::cerr << "Failed to create process: " << GetLastError() << exePath << std::endl;
            return NULL;
        }

        CloseHandle(pi.hThread); // close thread handle since we only need the process handle
        return pi.hProcess;
    }

    HANDLE AccessSharedMemory(const std::string& memoryName, size_t expectedSize, int retries, int retryDelayMs)
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

    HANDLE CreateSharedMemory(DWORD totalSize, LPCSTR name)
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

    void* MapSharedMemory(HANDLE hMapFile, size_t size)
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

    // OTHER

    HANDLE DuplicateHandleToProcess(HANDLE sourceProcessHandle, HANDLE handleToDuplicate, HANDLE targetProcessHandle) {
        HANDLE duplicatedHandle = NULL;

        if (DuplicateHandle(
            sourceProcessHandle,        // Handle to source process
            handleToDuplicate,          // Handle to duplicate
            targetProcessHandle,        // Handle to target process
            &duplicatedHandle,          // Out parameter for duplicated handle
            0,                          // Access (0 = same as original)
            FALSE,                      // Do not inherit handle
            DUPLICATE_SAME_ACCESS       // Duplicate with the same access rights
        )) {
            return duplicatedHandle;
        }
        else
        {
            std::cerr << "Failed to duplicate handle: " << GetLastError() << std::endl;
            return NULL;
        }
    }

    std::string GetExecutableDir()
    {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::filesystem::path exePath(buffer);
        return exePath.parent_path().string();
    }


    int GetRectWidth(const RECT& rect)
    {
        return rect.right - rect.left;
    }

    int GetRectHeight(const RECT& rect)
    {
        return rect.bottom - rect.top;
    }
};

