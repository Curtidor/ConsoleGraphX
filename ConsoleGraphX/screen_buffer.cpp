#include "screen_buffer.h"

ScreenBuffer::~ScreenBuffer()
{
	delete[] buffer;
}