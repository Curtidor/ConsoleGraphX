#pragma once
#include <string>
#include <windows.h>
#include "screen_buffer.h"

namespace ConsoleGraphX_Interal
{
	class Screen
	{
	private:
		const short _m_width;
		const short _m_height;
		const short _m_pixelWidth;
		const short _m_pixelHeight;
		const short _m_debuggerHeight;
		std::size_t _m_bufferSizeB;
		std::size_t _m_bufferSize;
		ConsoleGraphX_Interal::ScreenBuffer* _m_screenBuffer;

		static Screen* _s_activeScreen;


	public:
		static const wchar_t s_pixel = L'\x2588';
		static const wchar_t s_transparentPixel = L'‎‎';

		Screen(short width, short height, short debuggerHeight, short fontWidth, short fontHeight);
		~Screen();

		void SetPixel(int x, int y, CHAR_INFO& s_pixel);
		void SetPixels(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest);
		void SetText(int x, int y, const std::string& text);

		static void SetPixel_A(int x, int y, CHAR_INFO& s_pixel);
		static void SetPixels_A(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest);
		static void SetText_A(int x, int y, const std::string& text);

		void SetCursorPosition(short x, short y);
		void SetConsoleName(const std::string& name);
		void SetConsoleWindowSize(short width, short height);
		void FillScreen(const CHAR_INFO& color);
		/*
		* This function should only be used to fill the screen black, make sure to test the values carefully
		*/
		void MemFillScreen(int color);
		void RandomFillScreen();

		int GetPixelWidth() const;
		int GetPixelHeight() const;
		int GetWidth() const;
		int GetHeight() const;

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
};

