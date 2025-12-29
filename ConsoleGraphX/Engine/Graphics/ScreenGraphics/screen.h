#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincontypes.h>
#include <string>
#include <array>
#include "Engine\Graphics\color.h"
#include "Engine\Graphics\palette.h"
#include "Engine\Graphics\ScreenGraphics\pixel_canvas.h"

namespace ConsoleGraphX_Internal
{

	// methods with the ending "A" act on the Active screen

	class Screen : public PixelCanvas
	{
	protected:
		static inline Screen* _s_activeScreen = nullptr;

	protected:
		uint16_t _m_pixelWidth;
		uint16_t _m_pixelHeight;

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
		Screen(uint16_t width, uint16_t height, uint16_t fontWidth, uint16_t fontHeight, std::unique_ptr<PixelBuffer> sBuffer);
		// Normal constructor
		Screen(uint16_t width, uint16_t height, uint16_t fontWidth, uint16_t fontHeight);


		bool DrawScreen();
		const bool WriteText(const std::string& text,uint16_t x, uint16_t y) const;

		void WriteTextColor(CHAR_INFO* text, uint16_t x, uint16_t y);

		int GetPixelWidth() const;
		int GetPixelHeight() const;

		bool SetNewScreenSize(uint16_t width, uint16_t height, uint16_t fontWidth, uint16_t fontHeight);
		void SetScreenBuffer(ConsoleGraphX_Internal::PixelBuffer buffer);

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
