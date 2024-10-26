#include "PCH_CGX.h"
#include <winnt.h>
#include "pixel_buffer.h" 
#include "pixel_buffer_shared.h"

namespace ConsoleGraphX_Internal
{
    PixelBufferShared::PixelBufferShared(HANDLE console, CHAR_INFO* buffer, short width, short height)
        : PixelBufferHandle(console, width, height, buffer)
    {
        if (!_m_buffer)
        {
            throw std::runtime_error("Invalid shared memory buffer pointer");
        }
    }
}
