#include "screen_buffer_base.h" // has windows header
#include <wincontypes.h>
#include <winnt.h>
#include <stdexcept>
#include "screen_buffer_shared.h"


namespace ConsoleGraphX_Internal
{
    ScreenBufferShared::ScreenBufferShared(HANDLE console, CHAR_INFO* buffer, short width, short height)
        : ScreenBufferBase(console, width, height),
        _m_buffer(buffer)
    {
        if (!_m_buffer)
        {
            throw std::runtime_error("Invalid shared memory buffer pointer");
        }
    }

    CHAR_INFO* ScreenBufferShared::GetBuffer() const
    {
        return _m_buffer;
    }
}
