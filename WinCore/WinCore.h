#pragma once
#include <windows.h>
#include <string>
#include <vector>


bool SetConsoleFontSizeWC(HANDLE handle, short width, short height);

void SetCursorPositionWC(HANDLE, short x, short y);
void SetConsoleNameWC(const std::string& name);
void SetConsoleWindowSizeWC(HANDLE handle, short width, short height);
void DisableConsoleResize();
void RemoveConsoleTitleBar();


HANDLE CreateProcessWC(const std::string& exePath, const std::vector<std::string>& args);
HANDLE AccessSharedMemoryWC(const std::string& memoryName, size_t expectedSize, int retries=50, int retryDelayMs=50);
void* MapSharedMemoryWC(HANDLE hMapFile, size_t size);

/**
 * @brief Duplicates a handle from one process to another.
 *
 * @param sourceProcessHandle Handle to the process that owns the original handle.
 * @param handleToDuplicate The handle to duplicate from the source process.
 * @param targetProcessHandle Handle to the process where the duplicated handle will be valid.
 * @return Duplicated handle in the target process, or NULL if duplication fails.
 */
HANDLE DuplicateHandleToProcess(HANDLE sourceProcessHandle, HANDLE handleToDuplicate, HANDLE targetProcessHandle);

std::string GetExecutableDir();

int GetRectWidth(const RECT& rect);
int GetRectHeight(const RECT& rect);

template <typename T>
std::pair<T*, HMODULE> LoadModule(const std::string& dllPath, const std::string& createFuncName)
{
    HMODULE moduleHandle = LoadLibraryA(dllPath.c_str());
    if (!moduleHandle)
    {
        std::cerr << "Failed to load DLL: " << dllPath << std::endl;
        return {};
    }

    using CreateFuncType = T * (*)();
    auto createFunc = reinterpret_cast<CreateFuncType>(GetProcAddress(moduleHandle, createFuncName.c_str()));
    if (!createFunc)
    {
        std::cerr << "Failed to find factory function: " << createFuncName << " in DLL." << std::endl;
        FreeLibrary(moduleHandle);
        return {};
    }

    return { createFunc(), moduleHandle };
}

