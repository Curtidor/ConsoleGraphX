#pragma once
#include <string.h>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include "screen_buffer.h"

class Screen
{
private:
	const short _m_width;
	const short _m_height;
	const short _m_pixel_width;
	const short _m_pixel_height;
	const short _m_debugger_height;
	ScreenBuffer* _m_screenBuffer;
	
	static Screen* _s_active_screen;


public:
	static const wchar_t pixel = L'█';
	static const wchar_t noDrawChar = L'‎‎';

	Screen(short width, short height, short debugger_height, short fontWidth, short fontHeight);
	~Screen();

	void SetPixel(int x, int y, CHAR_INFO pixel);
	void SetPixels(CHAR_INFO* src_start, CHAR_INFO* src_end, CHAR_INFO* dest);
	void SetText(int x, int y, const std::string& text);
	
	static void SetPixel_A(int x, int y, CHAR_INFO pixel);
	static void SetPixels_A(CHAR_INFO* src_start, CHAR_INFO* src_end, CHAR_INFO* dest);
	static void SetText_A(int x, int y, const std::string& text);

	void SetCursorPosition(short x, short y);
	void SetConsoleName(const std::string& name);
	void SetConsoleWindowSize(short width, short height);
	void FillScreen(const CHAR_INFO& color);
	void RandomFillScreen();

	int GetPixelWidth();
	int GetPixelHeight();
	int GetWidth();
	int GetHeight();

	static int GetWidth_A();
	static int GetHeight_A();

	bool DrawScreen();
	bool SetConsoleFontSize(short width, short height);

	HWND GetConsoleWindowHandle();
	
	static Screen* GetActiveScreen_A();
	static void SetActiveScreen_A(Screen* screen);
	static CHAR_INFO* GetActiveScreenBuffer_A();
	static WORD RandomColor();

};