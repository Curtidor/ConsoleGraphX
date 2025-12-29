#include "PCH_WCore.h"
#include "WinCore.h"

// NOTE:
// - Console sizes are in CELLS (cols/rows).
// - Scroll bars appear when buffer > window.
// - We enforce buffer==window and also force-hide scroll bars on the HWND.

namespace WinCore
{
    // ---- small helpers ----
    static SMALL_RECT MakeRect(uint16_t cols, uint16_t rows) noexcept
    {
        SMALL_RECT r{};
        r.Left = 0;
        r.Top = 0;
        r.Right = (SHORT)std::max<int>(0, (int)cols - 1);
        r.Bottom = (SHORT)std::max<int>(0, (int)rows - 1);
        return r;
    }

    static bool GetConsoleCellSizes(HANDLE hOut, uint16_t& outWinCols, uint16_t& outWinRows, uint16_t& outBufCols, uint16_t& outBufRows) noexcept
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi{};
        if (!GetConsoleScreenBufferInfo(hOut, &csbi))
            return false;

        outBufCols = (uint16_t)csbi.dwSize.X;
        outBufRows = (uint16_t)csbi.dwSize.Y;

        outWinCols = (uint16_t)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
        outWinRows = (uint16_t)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
        return true;
    }

    // ---- WINDOW METRICS ----
    bool GetWindowOuterRectPx(HWND hWnd, WindowRectPx& out) noexcept
    {
        if (!hWnd) return false;

        RECT r{};
        if (!::GetWindowRect(hWnd, &r))
            return false;

        out.left = r.left;
        out.top = r.top;
        out.right = r.right;
        out.bottom = r.bottom;
        return true;
    }

    bool GetWindowOuterSizePx(HWND hWnd, int& outW, int& outH) noexcept
    {
        WindowRectPx r{};
        if (!GetWindowOuterRectPx(hWnd, r))
            return false;

        outW = r.Width();
        outH = r.Height();
        return true;
    }

    bool GetWindowOuterPosSizePx(HWND hWnd, int& outX, int& outY, int& outW, int& outH) noexcept
    {
        WindowRectPx r{};
        if (!GetWindowOuterRectPx(hWnd, r))
            return false;

        outX = r.left;
        outY = r.top;
        outW = r.Width();
        outH = r.Height();
        return true;
    }

    // ---- SCROLL BAR REMOVAL ----
    // Force-hide the scroll bars on a window (console or otherwise) without resizing/moving.
    // This is useful because the console host can still show scrollbars depending on state.
    bool HideWindowScrollBars_NoResize(HWND hWnd) noexcept
    {
        if (!hWnd) return false;

        // Hide both scrollbars visually.
        ::ShowScrollBar(hWnd, SB_BOTH, FALSE);

        // Also strip scrollbar styles if present.
        // (Console windows may or may not honor these permanently, but it helps.)
        SetLastError(0);
        LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
        if (!style && GetLastError() != 0)
            return false;

        style &= ~(WS_VSCROLL | WS_HSCROLL);

        SetLastError(0);
        if (!SetWindowLongPtr(hWnd, GWL_STYLE, style) && GetLastError() != 0)
            return false;

        // Recalc non-client area WITHOUT size/move changes.
        if (!SetWindowPos(
            hWnd, nullptr, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE))
        {
            return false;
        }

        return true;
    }

    // For consoles: ensure buffer == window (no scroll region) and then hide scrollbars on HWND.
    // Call this AFTER you've set buffer/window sizes.
    bool DisableConsoleScrollBars(HANDLE hOut) noexcept
    {
        if (!hOut || hOut == INVALID_HANDLE_VALUE)
            return false;

        uint16_t winC = 0, winR = 0, bufC = 0, bufR = 0;
        if (!GetConsoleCellSizes(hOut, winC, winR, bufC, bufR))
            return false;

        // If buffer larger than window, shrink it to match window.
        // (This is the actual root cause of scrollbars in console.)
        if (bufC != winC || bufR != winR)
        {
            // Rule: buffer cannot be smaller than window; we are making it equal to window, so OK.
            COORD newBuf{ (SHORT)winC, (SHORT)winR };
            if (!SetConsoleScreenBufferSize(hOut, newBuf))
                return false;
        }

        HWND hWnd = GetConsoleWindow();
        if (hWnd)
        {
            // Force-hide and strip scrollbar styles.
            (void)HideWindowScrollBars_NoResize(hWnd);
        }
        return true;
    }

    // ---- FONT ----
    ConsoleFontApplied SetConsoleFontSizeAndReadBack(HANDLE hOut, SHORT reqW, SHORT reqH, const wchar_t* face)
    {
        ConsoleFontApplied out{};

        CONSOLE_FONT_INFOEX fi{};
        fi.cbSize = sizeof(fi);

        if (!GetCurrentConsoleFontEx(hOut, FALSE, &fi))
        {
            out.lastError = GetLastError();
            return out;
        }

        fi.dwFontSize.X = reqW;
        fi.dwFontSize.Y = reqH;

        if (face && *face)
            wcsncpy_s(fi.FaceName, face, _TRUNCATE);

        if (!SetCurrentConsoleFontEx(hOut, FALSE, &fi))
        {
            out.lastError = GetLastError();
            return out;
        }

        // Read back what the host actually applied
        CONSOLE_FONT_INFOEX applied{};
        applied.cbSize = sizeof(applied);
        if (!GetCurrentConsoleFontEx(hOut, FALSE, &applied))
        {
            out.lastError = GetLastError();
            return out;
        }

        out.cellW = applied.dwFontSize.X;
        out.cellH = applied.dwFontSize.Y;
        wcsncpy_s(out.face, applied.FaceName, _TRUNCATE);
        out.ok = true;
        return out;
    }

    // ---- APPLY CONSOLE CONFIG ----
    ConsoleApplied ApplyConsoleConfig(HANDLE hOut, const ConsoleConfig& cfg)
    {
        ConsoleApplied out{};
        if (!hOut || hOut == INVALID_HANDLE_VALUE)
        {
            out.lastError = ERROR_INVALID_HANDLE;
            return out;
        }

        // 1) Font first, read-back actual applied cell size
        ConsoleFontApplied fontApp = SetConsoleFontSizeAndReadBack(hOut, cfg.fontW, cfg.fontH, L"Lucida Console");
        if (!fontApp.ok)
        {
            out.lastError = fontApp.lastError ? fontApp.lastError : GetLastError();
            return out;
        }
        out.fontW = fontApp.cellW;
        out.fontH = fontApp.cellH;

        // 2) Clamp requested cols/rows to max that fits this font + screen
        COORD largest = GetLargestConsoleWindowSize(hOut);
        uint16_t cols = std::max<uint16_t>(1, cfg.cols);
        uint16_t rows = std::max<uint16_t>(1, cfg.rows);

        if (largest.X > 0) cols = (uint16_t)std::min<int>(cols, largest.X);
        if (largest.Y > 0) rows = (uint16_t)std::min<int>(rows, largest.Y);

        // 3) Shrink window to 1x1 so buffer resize can't fail
        SMALL_RECT tiny{ 0,0,0,0 };
        if (!SetConsoleWindowInfo(hOut, TRUE, &tiny))
        {
            out.lastError = GetLastError();
            return out;
        }

        // 4) Set buffer to exact target
        COORD newBuf{ (SHORT)cols, (SHORT)rows };
        if (!SetConsoleScreenBufferSize(hOut, newBuf))
        {
            out.lastError = GetLastError();
            return out;
        }

        // 5) Set visible window to exact target
        SMALL_RECT win = MakeRect(cols, rows);
        if (!SetConsoleWindowInfo(hOut, TRUE, &win))
        {
            out.lastError = GetLastError();
            return out;
        }

        if (!cfg.title.empty())
            SetConsoleTitleA(cfg.title.c_str());

        // Optional: disable user resize
        if (cfg.disableResize)
            WinCore::DisableConsoleResize();

        // IMPORTANT: Kill scrollbars AFTER final buffer/window is applied
        (void)WinCore::DisableConsoleScrollBars(hOut);

        // Cache HWND + applied cell size (read back real outcome)
        out.hwnd= WinCore::WaitForConsoleHWND(); // ensure the console window is ready

        uint16_t winC = 0, winR = 0, bufC = 0, bufR = 0;
        if (GetConsoleCellSizes(hOut, winC, winR, bufC, bufR))
        {
            out.cols = winC;
            out.rows = winR;
            out.bufCols = bufC;
            out.bufRows = bufR;
        }
        else
        {
            // fallback to requested
            out.cols = cols;
            out.rows = rows;
            out.bufCols = cols;
            out.bufRows = rows;
        }

        // Borderless must NOT resize (your implementation already uses NOSIZE/NOMOVE)
        if (cfg.borderless && out.hwnd)
            WinCore::ApplyBorderlessStyle_NoResize(out.hwnd);

        return out;
    }

    // ---- BORDERLESS ----
    bool ApplyBorderlessStyle_NoResize(HWND hWnd) noexcept
    {
        if (!hWnd) return false;

        LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
        if (!style && GetLastError() != 0) return false;

        style &= ~(WS_CAPTION | WS_THICKFRAME);
        style &= ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);

        SetLastError(0);
        if (!SetWindowLongPtr(hWnd, GWL_STYLE, style) && GetLastError() != 0)
            return false;

        LONG_PTR ex = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
        if (!ex && GetLastError() != 0) return false;

        ex &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);

        SetLastError(0);
        if (!SetWindowLongPtr(hWnd, GWL_EXSTYLE, ex) && GetLastError() != 0)
            return false;

        if (!SetWindowPos(
            hWnd, nullptr,
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE))
        {
            return false;
        }

        // After changing frame, hide scrollbars again just in case.
        (void)HideWindowScrollBars_NoResize(hWnd);

        return true;
    }

    // Old simple setter (keep if other code calls it)
    bool SetConsoleFontSize(HANDLE handle, short width, short height)
    {
        CONSOLE_FONT_INFOEX font = { sizeof(CONSOLE_FONT_INFOEX) };
        if (!GetCurrentConsoleFontEx(handle, FALSE, &font))
            return false;

        font.dwFontSize.X = width;
        font.dwFontSize.Y = height;
        wcscpy_s(font.FaceName, L"Lucida Console");

        return SetCurrentConsoleFontEx(handle, FALSE, &font) != FALSE;
    }

    // ---- SetWindowPos wrappers ----
    bool MoveWindowNoSize(HWND hWnd, int x, int y, bool show) noexcept
    {
        if (!hWnd) return false;

        UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE;
        if (show) flags |= SWP_SHOWWINDOW;

        return SetWindowPos(hWnd, nullptr, x, y, 0, 0, flags) != FALSE;
    }

    bool SetWindowZOrderOnly(HWND hWnd, HWND insertAfter, bool show) noexcept
    {
        if (!hWnd) return false;

        UINT flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER;
        if (show) flags |= SWP_SHOWWINDOW;

        return SetWindowPos(hWnd, insertAfter, 0, 0, 0, 0, flags) != FALSE;
    }

    bool ResizeWindowOuterOnly(HWND hWnd, int width, int height, bool show) noexcept
    {
        if (!hWnd) return false;
        if (width < 1)  width = 1;
        if (height < 1) height = 1;

        UINT flags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE;
        if (show) flags |= SWP_SHOWWINDOW;

        return SetWindowPos(hWnd, nullptr, 0, 0, width, height, flags) != FALSE;
    }

    // ---- Console helpers ----
    void SetCursorPosition(HANDLE handle, short x, short y)
    {
        SetConsoleCursorPosition(handle, COORD{ x, y });
    }

    void SetConsoleName(const std::string& name)
    {
        SetConsoleTitleA(name.c_str());
    }

    int SetConsoleWindowSize(HANDLE handle, short width, short height)
    {
        // Right/Bottom are inclusive indices, so subtract 1
        SMALL_RECT rect = { 0, 0, (SHORT)(width - 1), (SHORT)(height - 1) };
        bool consoleWasSet = SetConsoleWindowInfo(handle, TRUE, &rect);

        return consoleWasSet ? 0 : (int)GetLastError();
    }

    void DisableConsoleResize()
    {
        HWND consoleWindow = GetConsoleWindow();
        if (!consoleWindow)
            return;

        LONG_PTR style = GetWindowLongPtr(consoleWindow, GWL_STYLE);
        style &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX);
        SetWindowLongPtr(consoleWindow, GWL_STYLE, style);

        SetWindowPos(consoleWindow, NULL, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE);

        // Optional: hide scrollbars too
        (void)HideWindowScrollBars_NoResize(consoleWindow);
    }

    void RemoveConsoleTitleBar()
    {
        HWND consoleWindow = GetConsoleWindow();
        if (!consoleWindow)
            return;

        LONG_PTR style = GetWindowLongPtr(consoleWindow, GWL_STYLE);
        style &= ~(WS_CAPTION | WS_THICKFRAME);
        style &= ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        SetWindowLongPtr(consoleWindow, GWL_STYLE, style);

        SetWindowPos(consoleWindow, NULL, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOACTIVATE);

        (void)HideWindowScrollBars_NoResize(consoleWindow);
    }

    // ---- IPC HELPERS ----
    HANDLE CGXCreateProcess(const std::string& exePath, const std::vector<std::string>& args)
    {
        std::string commandLine = exePath;
        for (const auto& arg : args)
            commandLine += " " + arg;

        std::vector<char> commandLineBuffer(commandLine.begin(), commandLine.end());
        commandLineBuffer.push_back('\0');

        STARTUPINFOA si = { sizeof(STARTUPINFOA) };
        PROCESS_INFORMATION pi = {};

        if (!CreateProcessA(NULL, commandLineBuffer.data(), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
        {
            std::cerr << "Failed to create process: " << GetLastError() << " " << exePath << std::endl;
            return NULL;
        }

        CloseHandle(pi.hThread);
        return pi.hProcess;
    }

    HANDLE AccessSharedMemory(const std::string& memoryName, size_t expectedSize, int retries, int retryDelayMs)
    {
        (void)expectedSize; // if you’re not verifying size here, remove the param later.
        HANDLE hMapFile = NULL;

        while (retries > 0)
        {
            hMapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, memoryName.c_str());
            if (hMapFile != NULL)
                break;

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
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            totalSize,
            name);

        if (hMapFile == NULL)
            std::cerr << "Could not create file mapping object: " << GetLastError() << std::endl;

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

    // ---- OTHER ----
    HANDLE DuplicateHandleToProcess(HANDLE sourceProcessHandle, HANDLE handleToDuplicate, HANDLE targetProcessHandle)
    {
        HANDLE duplicatedHandle = NULL;

        if (DuplicateHandle(
            sourceProcessHandle,
            handleToDuplicate,
            targetProcessHandle,
            &duplicatedHandle,
            0,
            FALSE,
            DUPLICATE_SAME_ACCESS))
        {
            return duplicatedHandle;
        }

        std::cerr << "Failed to duplicate handle: " << GetLastError() << std::endl;
        return NULL;
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
} 
