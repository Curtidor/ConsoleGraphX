#include "screen_buffer.h"

namespace ConsoleGraphX_Interal
{
	ScreenBuffer::~ScreenBuffer()
	{
		delete[] buffer;
	}
};
