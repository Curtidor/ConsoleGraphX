#pragma once
#include "Engine\Graphics\ScreenGraphics\pixel_buffer.h"

namespace ConsoleGraphX_Internal
{
	class PixelCanvas
	{
	protected:
		uint16_t _m_width;
		uint16_t _m_height;
		std::unique_ptr<PixelBuffer> _m_screenBuffer;

	public:
		PixelCanvas(uint16_t width, uint16_t height);
		PixelCanvas(uint16_t width, uint16_t height, std::unique_ptr<PixelBuffer> sBuffer);

		//Getters 
		uint16_t GetWidth() const;
		uint16_t GetHeight() const;

		// Buffer manipulation
		void SetPixel(unsigned int x, unsigned int y, CHAR_INFO s_pixel);
		void SetPixels(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest);
		void FillCanvas(CHAR_INFO fillChar);
	};

};