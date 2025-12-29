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
//external
#include "../WinCore/WinCore.h"
#include "../External/TermLog/Client/client.h"
//
#include "Engine/Core/Window/shared_window_memory.h"
#include "Engine/Graphics/ScreenGraphics/screen.h"
#include "Engine/Graphics/ScreenGraphics/pixel_buffer.h"
#include "Engine/Core/Window/abstract_window.h"
#include "console_handler.h"
#include "WindowStyleFlags.h"



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
static void InitializeConsole(HANDLE hConsole, uint16_t screenWidth, uint16_t screenHeight,
   uint16_t fontWidth, uint16_t fontHeight, const char* appName)
{   
    HWND windowHWND = GetConsoleWindow();

    WinCore::ConsoleConfig config{};
    config.cols = screenWidth;
    config.rows = screenHeight;
    config.fontW = fontWidth;
    config.fontH = fontHeight;
	config.title = appName;
    
    if (strcmp(appName, "Editor") != 0)
    {
        WinCore::ApplyWindowStyles_NoResize(windowHWND, WinCore::WindowStyleFlags::Borderless);
        config.borderless = true;

    }
    
    config.disableResize = true;

	WinCore::ConsoleApplied configApp = WinCore::ApplyConsoleConfig(hConsole, config);

    if (configApp.lastError != 0)
    {
        std::cerr << "Failed to apply console config. Error: " << configApp.lastError << std::endl;
        throw std::runtime_error("Fatal: failed to apply console config");
	}


}

static HANDLE InitializeSharedMemory(const char* windowName, DWORD sharedMemorySize,
    SharedWindowMemory*& sharedMem, uint16_t screenWidth, uint16_t screenHeight)
{
    HANDLE hMapFile = WinCore::CreateSharedMemory(sharedMemorySize, windowName);
    if (!hMapFile) return nullptr;

    sharedMem = static_cast<SharedWindowMemory*>(WinCore::MapSharedMemory(hMapFile, sharedMemorySize));
    if (!sharedMem) { CloseHandle(hMapFile); return nullptr; }

    new (sharedMem) SharedWindowMemory(screenWidth * screenHeight);

    // clear the backing buffer 
    CHAR_INFO* sharedBuffer = reinterpret_cast<CHAR_INFO*>(sharedMem + 1);
    std::memset(sharedBuffer, 0, sharedMem->m_bufferSize * sizeof(CHAR_INFO));
    return hMapFile;
}

/**
 * @file input_publishers.h
 * @brief Tiny, lock-free “publish” helpers for keyboard, mouse, size, and focus.
 *
 * ## Big Picture (for humans):
 * You have two programs (or two threads) sharing a memory block. The child process
 * (the console window) *writes* input state into that block. The parent process
 * (your engine) *reads* it. These functions run on the writer side and update that
 * shared memory safely without locks.
 *
 * - `SharedWindowMemory` contains:
 *      - 256 bits for which keys are down (8 x 32-bit chunks)
 *      - mouse position + button bitmask
 *      - current console columns/rows
 *      - focus flag
 *      - a “sequence number” (`inputSeq`) that bumps whenever anything changes
 *
 * - The reader can either:
 *      1) Just read fields and compare with last frame, or
 *      2) Use `inputSeq` to detect if anything changed while reading (read seq
 *         before and after, repeat if they differ).
 *
 * ## Why the “odd” memory orders?
 * - `memory_order_release` when we publish a final value says: “everything I wrote
 *   *before* this is now visible to any reader that does an `acquire` read.”
 * - `memory_order_acq_rel` on `fetch_add` means: when we bump the sequence, any reader
 *   that reads the sequence with `acquire` is guaranteed to see the published fields.
 * - `memory_order_relaxed` for independent, per-field numbers (like X/Y) is fine when
 *   we also do a release on a related “completion” write or seq bump.
 *
 * TL;DR: We publish values, then bump `inputSeq` with acq_rel so the reader can
 * acquire-load the sequence and trust the rest.
 */

 /// Compact bit math reminder:
 /// 256 keys = 8 × 32-bit integers
 /// key k (0..255) lives in chunk idx = floor(k / 32) and bit = k % 32
 /// We compute:
 ///   idx = (vk & 0xFF) >> 5     // divide by 32
 ///   bit = (vk & 0x1F)          // remainder mod 32

 /**
  * @brief Publish a single key’s up/down state into the shared-memory key bitset.
  *
  * This sets or clears *one* bit in an 8×32-bit array that represents all 256
  * virtual keys. If anything changed, we bump `inputSeq` so readers notice.
  *
  * @param s   Pointer to shared memory header.
  * @param vk  Virtual-key code (0..255). We mask with 0xFF to be safe.
  * @param down True if the key is currently pressed, false if released.
  *
  * @threading Writer-side only. Lock-free. No blocking calls.
  * @memory    - Reads current 32-bit chunk (relaxed).
  *            - Writes the new 32-bit chunk (release).
  *            - Bumps `inputSeq` (acq_rel) *only if the chunk actually changed*.
  *            The reader should use `acquire` when reading `inputSeq` (and/or the chunks)
  *            to observe a consistent view.
  *
  * @example
  * // VK_A (0x41 = 65) -> idx=2, bit=1
  * SetKey(shared, 0x41, true); // press A
  * SetKey(shared, 0x41, false); // release A
  */
inline void SetKey(SharedWindowMemory* s, unsigned vk, bool down)
{
    const unsigned idx = (vk & 0xFFu) >> 5;   // which 32-bit bucket (0..7)
    const unsigned bit = (vk & 0x1Fu);        // which bit inside that bucket (0..31)

    const uint32_t mask = 1u << bit;          // bit mask for this key
    const uint32_t cur = s->keyBits[idx].load(std::memory_order_relaxed);

    // Set or clear the bit depending on key state
    const uint32_t next = down ? (cur | mask) : (cur & ~mask);

    if (next != cur) {
        // Publish the new 32-bit word. 'release' => readers that do 'acquire'
        // will see this update once they also see the sequence bump below.
        s->keyBits[idx].store(next, std::memory_order_release);

        // Bump the sequence to announce "something changed".
        // acq_rel here pairs with reader’s 'acquire' load of inputSeq.
        s->inputSeq.fetch_add(1, std::memory_order_acq_rel);
    }
}

/**
 * @brief Publish current mouse state (position + button mask).
 *
 * We store position as 16-bit signed ints (console coords fit easily) and publish
 * a simple bitmask for buttons (e.g., 1<<0 = left, 1<<1 = right, etc.).
 *
 * @param s       Pointer to shared memory header.
 * @param x       Mouse X position in character cells.
 * @param y       Mouse Y position in character cells.
 * @param buttons Bitmask of pressed buttons (you define the mapping).
 *
 * @threading Writer-side only. Lock-free.
 * @memory    - X/Y stored relaxed (independent numbers).
 *            - Buttons stored with 'release' (acts like “I’m done publishing a coherent mouse snapshot”).
 *            - Sequence bumped with acq_rel so reader’s acquire sees everything.
 *
 * @note If you want an always-consistent snapshot, the reader can:
 *       do {
 *         seq0 = load_acquire(inputSeq);
 *         read x/y/buttons (acquire or relaxed is fine if guarded by seq)
 *         seq1 = load_acquire(inputSeq);
 *       } while (seq0 != seq1);
 */
inline void PublishMouse(SharedWindowMemory* s, int x, int y, uint32_t buttons)
{
    s->mouseX.store(static_cast<int16_t>(x), std::memory_order_relaxed);
    s->mouseY.store(static_cast<int16_t>(y), std::memory_order_relaxed);

    // Buttons as the final part of this “mouse snapshot”
    s->mouseButtons.store(buttons, std::memory_order_release);

    // Announce the update
    s->inputSeq.fetch_add(1, std::memory_order_acq_rel);
}

/**
 * @brief Publish the console’s current size (columns, rows).
 *
 * This reports the size of the *client area* in character cells, typically fed
 * by the console’s WINDOW_BUFFER_SIZE_EVENT.
 *
 * @param s Pointer to shared memory header.
 * @param c New column count (width in cells).
 * @param r New row count (height in cells).
 *
 * @threading Writer-side only. Lock-free.
 * @memory    - cols stored relaxed, rows stored with 'release' to mark snapshot end.
 *            - sequence bumped with acq_rel so reader sees both.
 *
 * @why       We publish `cols` then `rows` (release) so the reader that does
 *            acquire reads of either rows/seq (or seq twice) can trust the pair.
 */
inline void PublishSize(SharedWindowMemory* s, uint16_t c, uint16_t r)
{
    s->cols.store(c, std::memory_order_relaxed);
    s->rows.store(r, std::memory_order_release);
    s->inputSeq.fetch_add(1, std::memory_order_acq_rel);
}

/**
 * @brief Publish whether the console window currently has input focus.
 *
 * @param s        Pointer to shared memory header.
 * @param hasFocus True if focused, false if not.
 *
 * @threading Writer-side only. Lock-free.
 * @memory    - Focus stored with 'release' to close the “focus snapshot”.
 *            - Sequence bumped with acq_rel.
 *
 * @usage Reader can raise OnWindowFocusGained/Lost when this value flips.
 */
inline void PublishFocus(SharedWindowMemory* s, bool hasFocus)
{
    s->focus.store(hasFocus ? 1u : 0u, std::memory_order_release);
    s->inputSeq.fetch_add(1, std::memory_order_acq_rel);
}




/**
 * @brief Main rendering loop to manage screen updates and FPS display.
 *
 * @param screen Reference to the Screen object.
 * @param closeEvent event that signals console termination
 */
static void RunMainLoop(ConsoleGraphX_Internal::Screen& screen, HANDLE closeEvent, HANDLE hIn, SharedWindowMemory* sharedMem)
{
    int frameCount = 0;
    float fpsTimeCounter = 0.0f;
    auto startTime = std::chrono::high_resolution_clock::now();

    INPUT_RECORD recs[64];
    DWORD available = 0, read = 0;
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

        if (GetNumberOfConsoleInputEvents(hIn, &available) && available)
        {
            DWORD toRead = (available < 64) ? available : 64;
            if (ReadConsoleInput(hIn, recs, toRead, &read))
            {
                for (DWORD i = 0; i < read; ++i)
                {
					INPUT_RECORD rec = recs[i];
                    switch (rec.EventType) 
                    {
                    case KEY_EVENT: {
                        const auto& e = rec.Event.KeyEvent;
                        // optional: ignore repeats: (e.bKeyDown && (rec.Event.KeyEvent.dwControlKeyState & ENHANCED_KEY) == 0 && (rec.Event.KeyEvent.wRepeatCount > 1))
                        SetKey(sharedMem, e.wVirtualKeyCode, e.bKeyDown != 0);
                        break;
                    }
                    case MOUSE_EVENT: {
                        const auto& e = rec.Event.MouseEvent;
                        uint32_t btnMask = 0;
                        if (e.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) btnMask |= 1u << 0;
                        if (e.dwButtonState & RIGHTMOST_BUTTON_PRESSED)      btnMask |= 1u << 1;
                        if (e.dwButtonState & FROM_LEFT_2ND_BUTTON_PRESSED)  btnMask |= 1u << 2;
                        // (add more buttons if you need)
                        PublishMouse(sharedMem, e.dwMousePosition.X, e.dwMousePosition.Y, btnMask);
                        break;
                    }
                    case WINDOW_BUFFER_SIZE_EVENT: {
                        const COORD sz = rec.Event.WindowBufferSizeEvent.dwSize;
                        PublishSize(sharedMem, (uint16_t)sz.X, (uint16_t)sz.Y);
                        break;
                    }
                    case FOCUS_EVENT: {
                        PublishFocus(sharedMem, rec.Event.FocusEvent.bSetFocus != 0);
                        break;
                    }
                    default: break;
                    }

                }
            }
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

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);


   uint16_t screenWidth = static_cast<uint16_t>(std::stoi(argv[1]));
   uint16_t screenHeight = static_cast<uint16_t>(std::stoi(argv[2]));
   uint16_t fontWidth = static_cast<uint16_t>(std::stoi(argv[3]));
   uint16_t fontHeight = static_cast<uint16_t>(std::stoi(argv[4]));
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
    
    DWORD sharedMemorySize = CalcSharedSize(screenWidth, screenHeight);
    WinCore::ScopedHandle hMapFile(InitializeSharedMemory(windowName, sharedMemorySize, sharedMem,
        screenWidth, screenHeight));
    if (!hMapFile.get())
    {
        return 1;
    }

    WinCore::ScopedHandle hOut(GetStdHandle(STD_OUTPUT_HANDLE));
    if (hOut.get() == INVALID_HANDLE_VALUE)
    {
        UnmapViewOfFile(sharedMem);
        return 1;
    }

    CHAR_INFO* sharedBuffer = reinterpret_cast<CHAR_INFO*>(sharedMem + 1);

    auto sharedScreenBuffer = std::make_unique<ConsoleGraphX_Internal::PixelBufferHandle>(
        ConsoleGraphX_Internal::PixelBufferHandle(hOut.get(), screenWidth, screenHeight, sharedBuffer));

    ConsoleGraphX_Internal::Screen screen(screenWidth, screenHeight, fontWidth, fontHeight, std::move(sharedScreenBuffer));

    InitializeConsole(hOut.get(), screenWidth, screenHeight, fontWidth, fontHeight, windowName);
    
    HWND windowHWND = GetConsoleWindow();
    sharedMem->hwnd_bits = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(windowHWND));
    sharedMem->ready.store(1, std::memory_order_release);
   
    WinCore::ScopedHandle hIn(GetStdHandle(STD_INPUT_HANDLE));
    if (hIn.get() == INVALID_HANDLE_VALUE) 
    {
        UnmapViewOfFile(sharedMem);
        return 1;
    }

    // set INPUT console mode
    DWORD mode = 0; GetConsoleMode(hIn.get(), &mode);
    mode |= ENABLE_EXTENDED_FLAGS;
    mode |= ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT;
    mode &= ~ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(hIn.get(), mode);
    FlushConsoleInputBuffer(hIn.get());


    RunMainLoop(screen, closeEvent.get(), hIn.get(), sharedMem);

    // cleanup
    UnmapViewOfFile(sharedMem);
    SetEvent(cleanupEvent.get()); // signal that cleanup is complete

    return 0;
}

