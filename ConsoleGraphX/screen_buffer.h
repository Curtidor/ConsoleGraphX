#pragma once
#include "screen_buffer_base.h"
#include <winnt.h>
#include <wincontypes.h>

namespace ConsoleGraphX_Internal
{
    class ScreenBuffer : public ScreenBufferBase
    {
    private:
        CHAR_INFO* _m_buffer;    
    public:
        ScreenBuffer(HANDLE console, short width, short height);
        ~ScreenBuffer() override;

        CHAR_INFO* GetBuffer() const override;
    };
}
