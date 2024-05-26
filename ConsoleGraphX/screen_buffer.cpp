#include "screen_buffer.h"

namespace ConsoleGraphX_Internal
{
	ScreenBuffer::~ScreenBuffer()
	{
		delete[] buffer;
	}
};
