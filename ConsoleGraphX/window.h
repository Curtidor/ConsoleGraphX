#pragma once
#include <memory>
#include "abstract_window.h"
#include "shared_window_memory.h"
#include "pixel_buffer.h"
#include "screen.h"

namespace ConsoleGraphX
{
    class Window : public AbstractWindow, public ConsoleGraphX_Internal::Screen
    {
    public:
        Window(unsigned short width, unsigned short height, unsigned short fontWidth, unsigned short fontHeight, const std::string& windowName);
        
        virtual void Destroy() override;
        virtual void SetupWindow() override;
    };

    class CrossProcessWindow : public AbstractWindow, public ConsoleGraphX_Internal::Screen
    {
    private:
        HANDLE _m_hMapFile;
        HANDLE _m_processHandle;
        SharedWindowMemory* _m_sharedMem;

    public:
        CrossProcessWindow(unsigned short width, unsigned short height, unsigned short fontWidth, unsigned short fontHeight,
            const std::string& windowName, std::unique_ptr<ConsoleGraphX_Internal::PixelBuffer> sBuffer);

        virtual void Destroy() override;
        virtual void SetupWindow() override;

    private:
        void _CreateWindowImpl(short width, short height, short fontWidth, short fontHeight, const std::string& windowName);
        void _AccessSharedMemory();
    };
}
