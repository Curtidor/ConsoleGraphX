#pragma once
#include "pixel_buffer.h"

namespace ConsoleGraphX_Internal
{
	class PixelCanvas
	{
	protected:
		const unsigned short _m_width;
		const unsigned short _m_height;
		std::unique_ptr<PixelBuffer> _m_screenBuffer;

	public:
		PixelCanvas(unsigned short width, unsigned short height);
		PixelCanvas(unsigned short width, unsigned short height, std::unique_ptr<PixelBuffer> sBuffer);

		//Getters 
		unsigned short GetWidth() const;
		unsigned short GetHeight() const;

		// Buffer manipulation
		void SetPixel(unsigned int x, unsigned int y, CHAR_INFO s_pixel);
		void SetPixels(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest);
		void FillCanvas(CHAR_INFO fillChar);
	};

};