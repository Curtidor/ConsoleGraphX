#pragma once
#include <windows.h>
#include <wincontypes.h>

namespace ConsoleGraphX_Internal
{
    class PixelBuffer
    {
    public:
        COORD m_bufferSize;
        COORD m_bufferCoord;
        size_t m_size;
        SMALL_RECT m_writePosition;


    protected:
        CHAR_INFO* _m_buffer;   

    public:
        PixelBuffer(uint16_t width, uint16_t height);
        PixelBuffer(uint16_t width, uint16_t height, CHAR_INFO* buffer);
        virtual ~PixelBuffer() = default;

        // GETTERS
        COORD GetBufferSize() const;
        CHAR_INFO* GetBuffer() const;

        virtual HANDLE GetConsoleHandle() const; // Default is no handle
    };

    class PixelBufferHandle : public PixelBuffer
    {
    private:
        HANDLE _m_handle;

    public:
        PixelBufferHandle(HANDLE handle, uint16_t width, uint16_t height);
        PixelBufferHandle(HANDLE handle, uint16_t width, uint16_t height, CHAR_INFO* buffer);
        virtual ~PixelBufferHandle() = default;

        HANDLE GetConsoleHandle() const override;
    };
}
