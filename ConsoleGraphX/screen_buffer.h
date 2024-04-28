#pragma once
#include <windows.h>

namespace ConsoleGraphX_Interal
{
	struct ScreenBuffer
	{
		HWND wHandle;
		HANDLE hConsole;
		CHAR_INFO* buffer;
		COORD bufferSize;
		COORD bufferCoord;
		SMALL_RECT writePosition;

		~ScreenBuffer();
	};
};
