#pragma once
#include "pixel_buffer.h"
#include <wincontypes.h>
#include <winnt.h>

namespace ConsoleGraphX_Internal
{
    class PixelBufferShared : public PixelBufferHandle
    {

    public:
        PixelBufferShared(HANDLE console, CHAR_INFO* buffer, short width, short height);
        ~PixelBufferShared() override = default;
    };
}
