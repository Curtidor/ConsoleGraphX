#pragma once
#include <memory>
#include "Engine\Core\Window\abstract_window.h"
#include "Engine\Core\Window\shared_window_memory.h"
#include "Engine\Graphics\ScreenGraphics\pixel_buffer.h"
#include "Engine\Graphics\ScreenGraphics\screen.h"
#include "../../../../External/TermLog/Client/client.h"

namespace ConsoleGraphX
{
    class Window : public AbstractWindow
    {
    public:
        Window(uint16_t width, uint16_t height, uint16_t fontWidth, uint16_t fontHeight, const std::string& windowName);
        
        virtual void Destroy() override;
        virtual void SetupWindowImpl() override;
		void SetActiveRenderWindow();
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

    class CrossProcessWindow : public AbstractWindow
    {
    private:
        HANDLE _m_hMapFile;
        HANDLE _m_processHandle;
        SharedWindowMemory* _m_sharedMem;
		WindowState _m_windowState;
        bool _m_baselineInit = false;

    public:
        CrossProcessWindow(uint16_t width, uint16_t height, uint16_t fontWidth, uint16_t fontHeight,
            const std::string& windowName, std::unique_ptr<ConsoleGraphX_Internal::PixelBuffer> sBuffer);

        virtual void Destroy() override;
        virtual void SetupWindowImpl() override;
        virtual void TryResolveHWND() const noexcept override;

        void PollInput();

    private:
        void _CreateWindowImpl(short width, short height, short fontWidth, short fontHeight, const std::string& windowName);
        void _AccessSharedMemory();
    };
}
