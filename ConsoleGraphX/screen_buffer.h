#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincontypes.h>
#include "vector2.h"

namespace ConsoleGraphX_Internal
{
    struct ScreenBuffer
    {
    public:
        CHAR_INFO* m_buffer;
        SMALL_RECT m_writePosition;
        const HANDLE m_hConsole;
        const COORD m_bufferSize;
        const COORD m_bufferCoord;
        const size_t m_size;

        ScreenBuffer(HANDLE handle, const ConsoleGraphX::Vector2& size);
        ScreenBuffer(HANDLE handle, short width, short height);

        ~ScreenBuffer();
    };
}
