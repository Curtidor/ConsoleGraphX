#pragma once
#include <windows.h>

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