#include "PCH_CGX.h"
#include <processthreadsapi.h>
#include <filesystem>
#include <handleapi.h>
#include <memory>
#include <exception>
#include "Engine\Core\Window\window.h"
#include "Engine/Core/Logger/logger_manager.h"
#include "shared_window_memory.h"
#include "../WinCore/WinCore.h"
#include "../WinCore/console_handler.h"

namespace ConsoleGraphX
{
    // --- debug helpers ---
#define CGX_INPUT_DEBUG 0

#if CGX_INPUT_DEBUG
  #include <sstream>
  #include <iomanip>

  static inline std::string hex32(uint32_t v) 
  {
      std::ostringstream oss;
      oss << "0x" << std::uppercase << std::hex
          << std::setw(8) << std::setfill('0') << v;
      return oss.str();
  }
#endif

    
  // Window implementation
    Window::Window(uint16_t width, uint16_t height, uint16_t fontWidth, uint16_t fontHeight, const std::string& windowName)
		: AbstractWindow(windowName, ConsoleGraphX_Internal::Screen(width, height, fontWidth, fontHeight))
    {
        _m_windowHWND.store(GetConsoleWindow(), std::memory_order_release);
    }

    void Window::Destroy()
    {
        OnWindowDestroyed.Invoke(this);
    }

    void Window::SetActiveRenderWindow() 
    {
		ConsoleGraphX_Internal::Screen::SetActiveScreen_A(&_m_screen);
    }

    void Window::SetupWindowImpl()
    {
        std::string closeEventName = WINDOW_CLOSE_EVENT_NAME(std::string(_m_windowName));

        HANDLE closeEvent = CreateEventA(nullptr, TRUE, FALSE, closeEventName.c_str());
        if (closeEvent == INVALID_HANDLE_VALUE)
        {
            DWORD errorCode = GetLastError();
            std::cerr << "Failed to create close event. Error: " << errorCode << std::endl;
            throw std::runtime_error("Fatal: failed to make window close event");
        }

        std::function<void()> windowCloseCallback = [this, closeEvent]()
            {
                SetEvent(closeEvent);
            };

        WinCore::ConsoleHandler::RegisterCloseCallback(windowCloseCallback);
        WinCore::ConsoleHandler::SetHandler();

        _m_closeEvent = closeEvent;

        _m_screen.SetNewScreenSize(_m_screen.GetWidth(), _m_screen.GetHeight(), _m_screen.GetPixelWidth(), _m_screen.GetPixelHeight());
        
    }


	// CrossProcessWindow implementation
    CrossProcessWindow::CrossProcessWindow(uint16_t width, uint16_t height, uint16_t fontWidth, uint16_t fontHeight,
        const std::string& windowName, std::unique_ptr<ConsoleGraphX_Internal::PixelBuffer> sBuffer)
        : AbstractWindow(windowName, ConsoleGraphX_Internal::Screen(width, height, fontWidth, fontHeight, std::move(sBuffer))),
        _m_processHandle(INVALID_HANDLE_VALUE), _m_hMapFile(INVALID_HANDLE_VALUE), _m_sharedMem(nullptr), _m_windowState(WindowState{})
    {}

    void CrossProcessWindow::TryResolveHWND() const noexcept
    {
        if (!_m_sharedMem)
        {
            return;
        }

        if (_m_sharedMem->ready.load(std::memory_order_acquire) != 1)
        {
            return;
        }

        const uint64_t bits = _m_sharedMem->hwnd_bits;
        if (bits) 
        {
            const HWND h = reinterpret_cast<HWND>(static_cast<uintptr_t>(bits));
            _m_windowHWND.store(h, std::memory_order_release);
        }
       
        // last attempt
        if (!_m_windowHWND.load(std::memory_order_acquire))
        {
            _m_windowHWND.store(FindWindowA(nullptr, _m_windowName.c_str()), std::memory_order_release);
        }
    }

    void CrossProcessWindow::PollInput()
    {
        if (!_m_sharedMem || !_m_baselineInit)
        {
            return;
        }

        // resize
        uint16_t cols = _m_sharedMem->cols.load(std::memory_order_acquire);
        uint16_t rows = _m_sharedMem->rows.load(std::memory_order_relaxed);
        if (cols && rows && (cols != _m_windowState.cols || rows != _m_windowState.rows))
        {
            _m_windowState.cols = cols;
            _m_windowState.rows = rows;
            OnWindowResized.InvokeNFC(cols, rows);
			ConsoleGraphX_Internal::LoggerManager::Instance().LogMessage("CrossProcWindow", "Window resized to " + std::to_string(cols) + "x" + std::to_string(rows) + " for window: " + _m_windowName);
			//_m_client.PushMessage("Window resized to " + std::to_string(cols) + "x" + std::to_string(rows) + " for window: " + _m_windowName);
            // also call ResizeWindow(cols, rows) if you need to resize buffers
        }

        // Keys
        for (int i = 0; i < 8; ++i)
        {
            uint32_t cur = _m_sharedMem->keyBits[i].load(std::memory_order_acquire);
            uint32_t diff = cur ^ _m_windowState.keyBits[i];
            if (!diff)
            {
                continue;
            }
            for (int b = 0; b < 32; ++b)
            {
                if (diff & (1u << b))
                {
                    unsigned vk = (i << 5) | b;
                    bool down = (cur & (1u << b)) != 0;
                    uint32_t prev = _m_windowState.keyBits[i];
                    uint32_t mask = (1u << b);

                    if (down) {
                        OnKeyPressed.InvokeNF(vk);
                    }
                    else {
                        OnKeyReleased.InvokeNF(vk);
                    }

                #if CGX_INPUT_DEBUG
                    std::ostringstream os;
                    os << "Key " << vk << (down ? " DOWN" : " UP")
                        << " | chunk=" << i << " bit=" << b
                        << " mask=" << hex32(mask)
                        << " prev=" << hex32(prev)
                        << " cur=" << hex32(cur)
                        << " diff=" << hex32(diff)
                        << " seq=" << _m_sharedMem->inputSeq.load(std::memory_order_relaxed)
                        << " win=\"" << _m_windowName << "\"";
                    ConsoleGraphX_Internal::LoggerManager::Instance()
                        .LogMessage("CrossProcWindow", os.str(),
                            ConsoleGraphX_Internal::LoggerManager::LogLevel::CGX_INFO);
                #endif
                }
            }
            _m_windowState.keyBits[i] = cur;
        }

        // Focus
        uint32_t f = _m_sharedMem->focus.load(std::memory_order_acquire);
        if (f != _m_windowState.focus)
        {
            _m_windowState.focus = f;
            if (f)
            {
                OnWindowFocusGained.Invoke();
				//_m_client.PushMessage("Window focus gained for window: " + _m_windowName);
				ConsoleGraphX_Internal::LoggerManager::Instance().LogMessage("CrossProcWindow", "Window focus gained for window: " + _m_windowName);
                
            }
            else
            {
                OnWindowFocusLost.Invoke();
				//_m_client.PushMessage("Window focus lost for window: " + _m_windowName);
				ConsoleGraphX_Internal::LoggerManager::Instance().LogMessage("CrossProcWindow", "Window focus lost for window: " + _m_windowName);
            }
        }

        // TODO read and compare mouse move/click events
    }


    void CrossProcessWindow::Destroy()
    {
        bool terminationStatus = TerminateProcess(_m_processHandle, 0);
        CloseHandle(_m_processHandle);

        if (_m_sharedMem)
        {
            UnmapViewOfFile(_m_sharedMem);
        }

        if (_m_hMapFile)
        {
            CloseHandle(_m_hMapFile);
        }

        OnWindowDestroyed.Invoke(this);
    }

    void CrossProcessWindow::SetupWindowImpl()
    {
        _CreateWindowImpl(_m_screen.GetWidth(), _m_screen.GetHeight(), _m_screen.GetPixelWidth(), _m_screen.GetPixelHeight(), _m_windowName);
        _AccessSharedMemory();

        _m_windowHWND = FindWindowA(NULL, _m_windowName.c_str());


        // read HWND directly from shared memory
        _m_windowHWND = reinterpret_cast<HWND>(static_cast<uintptr_t>(_m_sharedMem->hwnd_bits));

        // hook the shared CHAR_INFO buffer (just past the header)
        CHAR_INFO* sharedBuffer = reinterpret_cast<CHAR_INFO*>(_m_sharedMem + 1);
       

        ConsoleGraphX_Internal::PixelBuffer pBuffer = { _m_screen.GetWidth(), _m_screen.GetHeight(), sharedBuffer };
        _m_screen.SetScreenBuffer(pBuffer);

        _m_windowState.cols = _m_sharedMem->cols.load(std::memory_order_acquire);
        _m_windowState.rows = _m_sharedMem->rows.load(std::memory_order_relaxed);
        _m_windowState.focus = _m_sharedMem->focus.load(std::memory_order_acquire);
        for (int i = 0; i < 8; ++i)
        {
            _m_windowState.keyBits[i] = _m_sharedMem->keyBits[i].load(std::memory_order_acquire);
        }

        _m_baselineInit = true;
    }


    void CrossProcessWindow::_CreateWindowImpl(short width, short height, short fontWidth, short fontHeight, const std::string& windowName)
    {
        std::string exePath = "WindowHandler.exe";
        std::vector args = {
            std::to_string(width),
            std::to_string(height),
            std::to_string(fontWidth),
            std::to_string(fontHeight),
            windowName
        };

        _m_processHandle = WinCore::CGXCreateProcess(exePath, args);
    }

    void CrossProcessWindow::_AccessSharedMemory()
    {
        const DWORD sharedSize = CalcSharedSize(_m_screen.GetWidth(), _m_screen.GetHeight());

		// attempt to access shared memory created by the child process (has built in retry logic)
        _m_hMapFile = WinCore::AccessSharedMemory(_m_windowName, sharedSize, 200, /*delayMs*/5);
        if (!_m_hMapFile)
        {
            throw std::runtime_error("Shared memory not available for window: " + _m_windowName);
        }

        _m_sharedMem = static_cast<SharedWindowMemory*>(
            MapViewOfFile(_m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sharedSize));

        if (!_m_sharedMem)
        {
            throw std::runtime_error("MapViewOfFile failed for window: " + _m_windowName);
        }

        // wait until child publishes readiness (release/acquire pair with the child)
        for (int i = 0; i < 2000; ++i) 
        {
            if (_m_sharedMem->ready.load(std::memory_order_acquire) == 1)
            {
                break;
            }
            Sleep(1);
        }

        if (_m_sharedMem->ready.load(std::memory_order_acquire) != 1)
        {
            throw std::runtime_error("Child window never became ready: " + _m_windowName);
        }

        // sanity check size
        const size_t expectedCells = static_cast<size_t>(_m_screen.GetWidth()) * static_cast<size_t>(_m_screen.GetHeight());
        if (_m_sharedMem->m_bufferSize != expectedCells)
        {
            throw std::runtime_error("Shared buffer size mismatch for window: " + _m_windowName);
        }
    }

}
