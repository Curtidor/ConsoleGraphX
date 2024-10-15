#include "CGXPCH.h"
#include "screen_buffer.h" // has windows header
#include "screen_buffer_base.h"


namespace ConsoleGraphX_Internal
{
    ScreenBuffer::ScreenBuffer(HANDLE console, short width, short height)
        : ScreenBufferBase(console, width, height),
        _m_buffer(new CHAR_INFO[width * height])
    {
        if (!_m_buffer)
        {
            throw std::runtime_error("Failed to allocate buffer");
        }
    }

    ScreenBuffer::~ScreenBuffer()
    {
        delete[] _m_buffer;  // Clean up dynamically allocated memory
    }

    CHAR_INFO* ScreenBuffer::GetBuffer() const
    {
        return _m_buffer;
    }
}
