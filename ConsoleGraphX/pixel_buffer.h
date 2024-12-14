#pragma once
#include <windows.h>
#include <wincontypes.h>

namespace ConsoleGraphX_Internal
{
    class PixelBuffer
    {
    public:
        const COORD m_bufferSize;
        const COORD m_bufferCoord;
        const size_t m_size;
        SMALL_RECT m_writePosition;


    protected:
        CHAR_INFO* _m_buffer;   

    public:
        PixelBuffer(short width, short height);
        PixelBuffer(short width, short height, CHAR_INFO* buffer);
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
        PixelBufferHandle(HANDLE handle, short width, short height);
        PixelBufferHandle(HANDLE handle, short width, short height, CHAR_INFO* buffer);
        virtual ~PixelBufferHandle() = default;

        HANDLE GetConsoleHandle() const override;
    };
}
