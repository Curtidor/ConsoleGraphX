#pragma once
#include <windows.h>
#include <string>
#include <vector>


bool SetConsoleFontSizeWC(HANDLE handle, short width, short height);

void SetCursorPositionWC(HANDLE, short x, short y);
void SetConsoleNameWC(const std::string& name);
void SetConsoleWindowSizeWC(HANDLE handle, short width, short height);

HANDLE CreateProcessWC(const std::string& exePath, const std::vector<std::string>& args);
HANDLE AccessSharedMemoryWC(const std::string& memoryName, size_t expectedSize, int retries=50, int retryDelayMs=50);
void* MapSharedMemoryWC(HANDLE hMapFile, size_t size);
