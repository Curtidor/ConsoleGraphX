#pragma once
#include <windows.h>
#include <wincontypes.h>

namespace ConsoleGraphX_Internal
{
    class ScreenBufferBase
    {
    public:
        SMALL_RECT m_writePosition;
        const COORD m_bufferSize;      
        const COORD m_bufferCoord;   
        const size_t m_size;

    protected:
        HANDLE _m_hConsole;      

    public:
        ScreenBufferBase(HANDLE console, short width, short height);
        virtual ~ScreenBufferBase() = default;

        HANDLE GetConsoleHandle() const;

        COORD GetBufferSize() const;

        virtual CHAR_INFO* GetBuffer() const = 0;
    };
}
