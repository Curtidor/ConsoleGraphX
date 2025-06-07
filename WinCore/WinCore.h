#pragma once
#include <iostream>
#include <windows.h>
#include <string>
#include <vector>

namespace WinCore
{
    class ScopedHandle
    {
    public:
        explicit ScopedHandle(HANDLE handle) : _m_handle(handle) {}
        ~ScopedHandle()
        {
            if (_m_handle && _m_handle != INVALID_HANDLE_VALUE)
            {
                CloseHandle(_m_handle);
            }
        }

        // disable copy to prevent double closing
        ScopedHandle(const ScopedHandle&) = delete;
        ScopedHandle& operator=(const ScopedHandle&) = delete;

        // enable move semantics
        ScopedHandle(ScopedHandle&& other) noexcept : _m_handle(other._m_handle)
        {
            other._m_handle = nullptr;
        }
        ScopedHandle& operator=(ScopedHandle&& other) noexcept
        {
            if (this != &other)
            {
                if (_m_handle && _m_handle != INVALID_HANDLE_VALUE)
                {
                    CloseHandle(_m_handle);
                }
                _m_handle = other._m_handle;
                other._m_handle = nullptr;
            }
            return *this;
        }

        HANDLE get() const { return _m_handle; }

    private:
        HANDLE _m_handle;
    };

    bool SetConsoleFontSize(HANDLE handle, short width, short height);

    void SetCursorPosition(HANDLE, short x, short y);
    void SetConsoleName(const std::string& name);
    int SetConsoleWindowSize(HANDLE handle, short width, short height);
    void DisableConsoleResize();
    void RemoveConsoleTitleBar();


    HANDLE CGXCreateProcess(const std::string& exePath, const std::vector<std::string>& args);
    HANDLE AccessSharedMemory(const std::string& memoryName, size_t expectedSize, int retries = 50, int retryDelayMs = 50);
    HANDLE CreateSharedMemory(DWORD totalSize, LPCSTR name);
    void* MapSharedMemory(HANDLE hMapFile, size_t size);

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

};


