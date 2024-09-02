#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincontypes.h>
#include "screen_buffer.h"
#include "vector2.h"

namespace ConsoleGraphX_Internal
{
    ScreenBuffer::ScreenBuffer(HANDLE handle, const ConsoleGraphX::Vector2& size)
        : m_hConsole(handle),
        m_size(static_cast<size_t>(size.x* size.y)),
        m_buffer(new CHAR_INFO[size.x * size.y]),
        m_bufferCoord{ 0, 0 },
        m_bufferSize{ static_cast<short>(size.x), static_cast<short>(size.y) },
        m_writePosition{ 0, 0, static_cast<short>(size.x - 1), static_cast<short>(size.y - 1) }
    {}

    ScreenBuffer::ScreenBuffer(HANDLE handle, short width, short height)
        : m_hConsole(handle),
        m_size(width* height),
        m_buffer(new CHAR_INFO[width * height]),
        m_bufferCoord{ 0, 0 },
        m_bufferSize{ width, height },
        m_writePosition{ 0, 0, static_cast<short>(width - 1), static_cast<short>(height - 1) }
    {}

    ScreenBuffer::~ScreenBuffer()
    {
        delete[] m_buffer;
    }
}

