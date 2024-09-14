#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincontypes.h>
#include <string>
#include "screen_buffer.h"
#include <array>

namespace ConsoleGraphX_Internal
{
	struct RGB_CGX
	{
		unsigned short r;
		unsigned short g;
		unsigned short b;
	};

	class Screen
	{
	private:
		const short _m_width;
		const short _m_height;
		const short _m_pixelWidth;
		const short _m_pixelHeight;
		ScreenBuffer* _m_screenBuffer;

		static Screen* _s_activeScreen;


	public:									   
		static const wchar_t s_pixel = L'\x2588';
		static const wchar_t s_transparentPixel = L'‎';

		Screen(short width, short height, short fontWidth, short fontHeight);
		~Screen();

		bool DrawScreen();
		bool SetConsoleFontSize(short width, short height);

		void SetPixel(int x, int y, CHAR_INFO s_pixel);
		void SetPixels(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest);
		void SetText(int x, int y, const std::string& text);

		void SetCursorPosition(short x, short y);
		void SetConsoleName(const std::string& name);
		void SetConsoleWindowSize(short width, short height);
		void FillScreen(const CHAR_INFO& color);
		void RandomFillScreen();

		int GetPixelWidth() const;
		int GetPixelHeight() const;
		int GetWidth() const;
		int GetHeight() const;

		static int GetWidth_A();
		static int GetHeight_A();
		
		static void SetPalletColors(const std::array<RGB_CGX, 16>& colors);
		static void SetPixel_A(int x, int y, CHAR_INFO s_pixel);
		static void SetPixels_A(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest);
		static void SetText_A(int x, int y, const std::string& text);
		static void SetActiveScreen_A(Screen* screen);

		static Screen* GetActiveScreen_A();
		static CHAR_INFO* GetActiveScreenBuffer_A();

		static WORD RandomColor();

	};
};
