#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincontypes.h>
#include <string>
#include <array>
#include "screen_buffer_base.h"
#include "color.h"
#include "palette.h"
#include "screen_buffer_shared.h"

namespace ConsoleGraphX_Internal
{
	class Screen
	{
	private:
		const short _m_width;
		const short _m_height;
		const short _m_pixelWidth;
		const short _m_pixelHeight;
		ScreenBufferBase* _m_screenBuffer;

		static Screen* _s_activeScreen;


	public:									   
		static const wchar_t s_pixel = L'\x2588';
		static const wchar_t s_transparentPixel = L'‎';

		// Shared memory constructor
		Screen(short width, short height, short fontWidth, short fontHeight, ScreenBufferShared* screenBuffer);
		// Normal constructor
		Screen(short width, short height, short fontWidth, short fontHeight);

		~Screen();

		bool DrawScreen();
		bool SetConsoleFontSize(short width, short height);

		void SetPixel(int x, int y, CHAR_INFO s_pixel);
		void SetPixels(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest);

		void SetCursorPosition(short x, short y);
		void SetConsoleName(const std::string& name);
		void SetConsoleWindowSize(short width, short height);
		void FillScreen(const CHAR_INFO& color);

		int GetPixelWidth() const;
		int GetPixelHeight() const;
		int GetWidth() const;
		int GetHeight() const;

		static int GetWidth_A();
		static int GetHeight_A();
		
		static void SetPalletColors_A(std::array<ConsoleGraphX::Color_CGX, 16>& paletteColors);
		static void SetPalletColors_A(ConsoleGraphX::Palette& paletteColors);
		static void SetPalletColor_A(const ConsoleGraphX::Color_CGX& color, int index);
		static void SetPixel_A(int x, int y, CHAR_INFO s_pixel);
		static void SetPixels_A(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest);
		static void SetActiveScreen_A(Screen* screen);

		static Screen* GetActiveScreen_A();
		static CHAR_INFO* GetActiveScreenBuffer_A();


	};
};
