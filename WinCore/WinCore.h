#pragma once

#include <Windows.h>
#include <atomic>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include <thread>

namespace ConsoleGraphX_Internal { class ThreadManager; }

namespace WinCore
{

    struct WindowRectPx
    {
        int left = 0;
        int top = 0;
        int right = 0;
        int bottom = 0;

        int Width()  const noexcept { return right - left; }
        int Height() const noexcept { return bottom - top; }
    };


    bool GetWindowOuterRectPx(HWND hWnd, WindowRectPx& out) noexcept;


    //=========================================================================
    // RAII HANDLE
    //=========================================================================
    class ScopedHandle
    {
    public:
        ScopedHandle() noexcept : _m_handle(nullptr) {}
        explicit ScopedHandle(HANDLE handle) noexcept : _m_handle(handle) {}

        ~ScopedHandle() noexcept
        {
            reset();
        }

        ScopedHandle(const ScopedHandle&) = delete;
        ScopedHandle& operator=(const ScopedHandle&) = delete;

        ScopedHandle(ScopedHandle&& other) noexcept : _m_handle(other._m_handle)
        {
            other._m_handle = nullptr;
        }

        ScopedHandle& operator=(ScopedHandle&& other) noexcept
        {
            if (this != &other)
            {
                reset();
                _m_handle = other._m_handle;
                other._m_handle = nullptr;
            }
            return *this;
        }

        HANDLE get() const noexcept { return _m_handle; }
        explicit operator bool() const noexcept { return _m_handle && _m_handle != INVALID_HANDLE_VALUE; }

        void reset(HANDLE h = nullptr) noexcept
        {
            if (_m_handle && _m_handle != INVALID_HANDLE_VALUE)
            {
                ::CloseHandle(_m_handle);
            }
            _m_handle = h;
        }

        HANDLE release() noexcept
        {
            HANDLE h = _m_handle;
            _m_handle = nullptr;
            return h;
        }

    private:
        HANDLE _m_handle;
    };

    //=========================================================================
    // WINDOW METRICS (OUTER RECT)
    //=========================================================================
    

    // Outer window bounds in screen pixels (includes borders/titlebar).
    bool GetWindowOuterRectPx(HWND hWnd, WindowRectPx& out) noexcept;

    // Convenience: just width/height of outer bounds.
    bool GetWindowOuterSizePx(HWND hWnd, int& outW, int& outH) noexcept;

    // Convenience: outer position (top-left) + size.
    bool GetWindowOuterPosSizePx(HWND hWnd, int& outX, int& outY, int& outW, int& outH) noexcept;

    //=========================================================================
    // WINDOW POSITIONING (SetWindowPos WRAPPERS)
    //=========================================================================
    // Move ONLY. Never resizes, never changes z-order, never activates.
    bool MoveWindowNoSize(HWND hWnd, int x, int y, bool show = true) noexcept;

    // Z-order ONLY. Never moves, never resizes, never activates.
    // insertAfter: HWND_TOP, HWND_BOTTOM, HWND_TOPMOST, HWND_NOTOPMOST, or another HWND.
    bool SetWindowZOrderOnly(HWND hWnd, HWND insertAfter, bool show = true) noexcept;

    // Resize ONLY (outer window rect). Never moves, never changes z-order, never activates.
    bool ResizeWindowOuterOnly(HWND hWnd, int width, int height, bool show = true) noexcept;

    //=========================================================================
    // SCROLLBARS / WINDOW CHROME
    //=========================================================================
    // Force-hide scrollbars without resizing/moving (also strips WS_VSCROLL/WS_HSCROLL if present).
    bool HideWindowScrollBars_NoResize(HWND hWnd) noexcept;

    // Makes the window borderless without changing its size or position.
    bool ApplyBorderlessStyle_NoResize(HWND hWnd) noexcept;

    //=========================================================================
    // CONSOLE CONFIG (FONT + BUFFER + WINDOW)
    //=========================================================================
    struct ConsoleFontApplied
    {
        SHORT cellW = 0;
        SHORT cellH = 0;
        wchar_t face[LF_FACESIZE]{};
        bool ok = false;
        DWORD lastError = 0;
    };

    struct ConsoleConfig
    {
        uint16_t cols = 120;
        uint16_t rows = 40;
        uint16_t fontW = 8;
        uint16_t fontH = 8;

        bool disableResize = true;
        bool borderless = false;

        std::string title;
    };

    struct ConsoleApplied
    {
        // Applied visible window size in cells (srWindow)
        uint16_t cols = 0;
        uint16_t rows = 0;

        // Applied screen buffer size in cells (dwSize)
        uint16_t bufCols = 0;
        uint16_t bufRows = 0;

        // Applied font cell size in pixels
        uint16_t fontW = 0;
        uint16_t fontH = 0;

        HWND hwnd = nullptr;
        DWORD lastError = 0;
    };

    // Set console font size and read back what was actually applied.
    ConsoleFontApplied SetConsoleFontSizeAndReadBack(HANDLE hOut, SHORT reqW, SHORT reqH, const wchar_t* face);

    // Apply full console config: font -> shrink window -> set buffer -> set window -> optional title/resize/borderless
    // Also disables scrollbars by enforcing buffer==window + forcing scrollbar hide.
    ConsoleApplied ApplyConsoleConfig(HANDLE hOut, const ConsoleConfig& cfg);

    // Enforce: buffer == visible window (kills scrollbars) + force-hide scrollbar visuals on HWND.
    bool DisableConsoleScrollBars(HANDLE hOut) noexcept;

    // Legacy/simple setter (kept for compatibility).
    bool SetConsoleFontSize(HANDLE handle, short width, short height);

    // Misc console helpers
    void SetCursorPosition(HANDLE handle, short x, short y);
    void SetConsoleName(const std::string& name);
    int  SetConsoleWindowSize(HANDLE handle, short width, short height); // width/height in cells
    void DisableConsoleResize();
    void RemoveConsoleTitleBar();

    //=========================================================================
    // IPC HELPERS
    //=========================================================================
    HANDLE CGXCreateProcess(const std::string& exePath, const std::vector<std::string>& args);
    HANDLE AccessSharedMemory(const std::string& memoryName, size_t expectedSize, int retries = 50, int retryDelayMs = 50);
    HANDLE CreateSharedMemory(DWORD totalSize, LPCSTR name);
    void* MapSharedMemory(HANDLE hMapFile, size_t size);

    HANDLE DuplicateHandleToProcess(HANDLE sourceProcessHandle, HANDLE handleToDuplicate, HANDLE targetProcessHandle);

    //=========================================================================
    // FILE / RECT UTILS
    //=========================================================================
    std::string GetExecutableDir();
    int GetRectWidth(const RECT& rect);
    int GetRectHeight(const RECT& rect);

    //=========================================================================
    // CONSOLE HWND WAIT (UTILITY)
    //=========================================================================
    inline HWND WaitForConsoleHWND(DWORD timeoutMs = 2000) noexcept
    {
        ULONGLONG start = GetTickCount64();
        while (GetTickCount64() - start < timeoutMs)
        {
            HWND h = GetConsoleWindow();
            if (h && IsWindow(h))
                return h;
            Sleep(1);
        }
        return nullptr;
    }

    //=========================================================================
    // MODULE LOADING (KEEP THIS OUT OF HEADERS LATER)
    // NOTE: This template drags Win32 + strings into every TU. Fine for now.
    //=========================================================================
    template <typename T>
    inline std::pair<T*, HMODULE> LoadModule(const std::string& dllPath, const std::string& createFuncName)
    {
        HMODULE moduleHandle = LoadLibraryA(dllPath.c_str());
        if (!moduleHandle)
        {
            return {};
        }

        using CreateFuncType = T * (*)();
        auto createFunc = reinterpret_cast<CreateFuncType>(GetProcAddress(moduleHandle, createFuncName.c_str()));
        if (!createFunc)
        {
            FreeLibrary(moduleHandle);
            return {};
        }

        return { createFunc(), moduleHandle };
    }
} // namespace WinCore
