#include "screen_buffer_base.h" // has windows header
#include <wincontypes.h>
#include <winnt.h>



namespace ConsoleGraphX_Internal
{
    ScreenBufferBase::ScreenBufferBase(HANDLE console, short width, short height)
        : _m_hConsole(console),
        m_bufferCoord{ 0, 0 },
        m_bufferSize{ width, height },
        m_writePosition{ 0, 0, static_cast<short>(width - 1), static_cast<short>(height - 1) },
        m_size(width* height)
    {}

    HANDLE ScreenBufferBase::GetConsoleHandle() const
    {
        return _m_hConsole;
    }

    COORD ScreenBufferBase::GetBufferSize() const
    {
        return m_bufferSize;
    }
}
