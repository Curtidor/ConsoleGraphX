#pragma once
#include <string.h>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include "screen_buffer.h"

class Screen
{
private:
	const short m_width;
	const short m_height;
	const short m_pixel_width;
	const short m_pixel_height;
	const short m_debugger_height;
	ScreenBuffer* m_screenBuffer;
	
	static Screen* s_active_screen;


public:
	static const wchar_t pixel = L'█';

	Screen(short width, short height, short debugger_height, short fontWidth, short fontHeight);
	~Screen();

	void SetPixel(int x, int y, CHAR_INFO pixel);
	void SetPixels(CHAR_INFO* src_start, CHAR_INFO* src_end, CHAR_INFO* dest);
	static void SetPixel_A(int x, int y, CHAR_INFO pixel);
	static void SetPixels_A(CHAR_INFO* src_start, CHAR_INFO* src_end, CHAR_INFO* dest);
	void SetText(int x, int y, const std::string& text);
	static void SetText_A(int x, int y, const std::string& text);
	void SetCursorPosition(short x, short y);
	void SetConsoleName(const std::string& name);
	void SetConsoleWindowSize(short width, short height);
	void FillScreen(const CHAR_INFO& color);
	void RandomFillScreen();

	int GetPixelWidth();
	int GetWidth();
	int GetHeight();
	int GetPixelHeight();

	bool DrawScreen();
	bool SetConsoleFontSize(short width, short height);

	HWND GetConsoleWindowHandle();

	static int GetWidth_A();
	static int GetHeight_A();
	
	static Screen* GetActiveScreen();
	static CHAR_INFO* GetBuffer();
	static WORD RandomColor();

};