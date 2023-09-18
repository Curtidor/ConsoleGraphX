#pragma once
#include <windows.h>

struct ScreenBuffer
{
	HANDLE hConsole;
	HWND wHandle;
	CHAR_INFO* buffer;
	COORD bufferSize;
	COORD bufferCoord;
	SMALL_RECT writePosition;

	~ScreenBuffer();
};