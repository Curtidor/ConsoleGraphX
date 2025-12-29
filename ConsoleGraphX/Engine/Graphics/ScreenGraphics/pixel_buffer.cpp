#include "PCH_CGX.h"
#include "Engine\Graphics\ScreenGraphics\pixel_buffer.h"

namespace ConsoleGraphX_Internal
{
    PixelBuffer::PixelBuffer(uint16_t width, uint16_t height)
        : m_bufferSize( width, height ),
        m_bufferCoord({ 0, 0 }),
        m_size(width* height),
        m_writePosition( 0, 0, static_cast<uint16_t>(width - 1), static_cast<uint16_t>(height - 1) ),
        _m_buffer(new CHAR_INFO[width * height])
    {}

    PixelBuffer::PixelBuffer(uint16_t width, uint16_t height, CHAR_INFO* buffer)
        : m_bufferSize( width, height ),
        m_bufferCoord({ 0, 0 }),
        m_size(width* height),
        m_writePosition( 0, 0, static_cast<uint16_t>(width - 1), static_cast<uint16_t>(height - 1) ),
        _m_buffer(buffer)
    {}

    COORD PixelBuffer::GetBufferSize() const
    {
        return m_bufferSize;
    }

    CHAR_INFO* PixelBuffer::GetBuffer() const
    {
        return _m_buffer;
    }

    HANDLE PixelBuffer::GetConsoleHandle() const
    {
        return INVALID_HANDLE_VALUE; // Default is no handle
    }

    // --------------HANDLE CLASS--------------
    PixelBufferHandle::PixelBufferHandle(HANDLE handle, uint16_t width, uint16_t height)
        : PixelBuffer(width, height), _m_handle(handle)
    {}

    PixelBufferHandle::PixelBufferHandle(HANDLE handle, uint16_t width, uint16_t height, CHAR_INFO* buffer)
        : PixelBuffer(width, height, buffer), _m_handle(handle)
    {}

    HANDLE PixelBufferHandle::GetConsoleHandle() const
    {
        return _m_handle;
    }
}
