#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincontypes.h>
#include <string>
#include <array>
#include "color.h"
#include "palette.h"
#include "pixel_canvas.h"

namespace ConsoleGraphX_Internal
{
	class Screen : public PixelCanvas
	{
	protected:
		static inline Screen* _s_activeScreen = nullptr;

	protected:
		const unsigned short _m_pixelWidth;
		const unsigned short _m_pixelHeight;

	public:									   
		static const wchar_t s_pixel = L'\x2588';
		static const wchar_t s_transparentPixel = L'‎';

		/// <summary>
		/// Shared memory constructor
		/// Note: you must call the initialize function manually when using this constructor
		/// </summary>
		/// <param name="width"></param>
		/// <param name="height"></param>
		/// <param name="fontWidth"></param>
		/// <param name="fontHeight"></param>
		/// <param name="sBuffer"></param>
		Screen(unsigned short width, unsigned short height, unsigned short fontWidth, unsigned short fontHeight, std::unique_ptr<PixelBuffer> sBuffer);
		// Normal constructor
		Screen(unsigned short width, unsigned short height, unsigned short fontWidth, unsigned short fontHeight);


		bool DrawScreen();
		bool WriteText(const std::string& text, short x, short y);


		void Initialize();
		void WriteTextColor(CHAR_INFO* text, short x, short y);

		int GetPixelWidth() const;
		int GetPixelHeight() const;

		CHAR_INFO* GetScreenBuffer();

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
