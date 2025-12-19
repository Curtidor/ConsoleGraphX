#pragma once
#include <memory>
#include "Engine\Core\Window\abstract_window.h"
#include "Engine\Core\Window\shared_window_memory.h"
#include "Engine\Graphics\ScreenGraphics\pixel_buffer.h"
#include "Engine\Graphics\ScreenGraphics\screen.h"
#include "../../../../External/TermLog/Client/client.h"

namespace ConsoleGraphX
{
    class Window : public AbstractWindow, public ConsoleGraphX_Internal::Screen
    {
    public:
        Window(unsigned short width, unsigned short height, unsigned short fontWidth, unsigned short fontHeight, const std::string& windowName);
        
        virtual void Destroy() override;
        virtual void SetupWindow() override;

        virtual Vector2 GetTargetWindowSize() override;
        virtual Vector2 GetTargetWindowSizeInPixels() override;

    };

    struct WindowState
    {
        uint16_t cols, rows; // last buffer size
        int16_t  mouseX, mouseY;
        uint32_t mouseButtons; // bitmask
        uint32_t focus;        // 0/1
        uint64_t inputSeq;     // increment on any input change
        uint32_t keyBits[8];   // 256 keys -> 8 * 32-bit
    };

    class CrossProcessWindow : public AbstractWindow, public ConsoleGraphX_Internal::Screen
    {
    private:
        HANDLE _m_hMapFile;
        HANDLE _m_processHandle;
        SharedWindowMemory* _m_sharedMem;
		WindowState _m_windowState;
		Client _m_client;
        bool _m_baselineInit = false;

    public:
        CrossProcessWindow(unsigned short width, unsigned short height, unsigned short fontWidth, unsigned short fontHeight,
            const std::string& windowName, std::unique_ptr<ConsoleGraphX_Internal::PixelBuffer> sBuffer);

        virtual void Destroy() override;
        virtual void SetupWindow() override;

        virtual void TryResolveHWND() const noexcept override;

        virtual Vector2 GetTargetWindowSize() override;
        virtual Vector2 GetTargetWindowSizeInPixels() override;

        void PollInput();

    private:
        void _CreateWindowImpl(short width, short height, short fontWidth, short fontHeight, const std::string& windowName);
        void _AccessSharedMemory();
    };
}
