#pragma once
#include "screen_buffer_base.h"
#include <wincontypes.h>
#include <winnt.h>

namespace ConsoleGraphX_Internal
{
    class ScreenBufferShared : public ScreenBufferBase
    {
    private:
        CHAR_INFO* _m_buffer; 

    public:
        ScreenBufferShared(HANDLE console, CHAR_INFO* buffer, short width, short height);
        ~ScreenBufferShared() override = default;

        CHAR_INFO* GetBuffer() const override;
    };
}
