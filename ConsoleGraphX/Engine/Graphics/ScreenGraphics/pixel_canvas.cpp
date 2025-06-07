#include "PCH_CGX.h"
#include "Engine\Graphics\ScreenGraphics\pixel_canvas.h"
#include "Engine\Graphics\ScreenGraphics\pixel_buffer.h"

namespace ConsoleGraphX_Internal
{
	PixelCanvas::PixelCanvas(unsigned short width, unsigned short height) // Used when making a canvas in the same process 
		: _m_width(width), _m_height(height), _m_screenBuffer(nullptr)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		if (hConsole == INVALID_HANDLE_VALUE)
			throw std::runtime_error("Failed to get the console handle");

		_m_screenBuffer = std::make_unique<PixelBufferHandle>(PixelBufferHandle(hConsole, width, height));
	}

	PixelCanvas::PixelCanvas(unsigned short width, unsigned short height, std::unique_ptr<PixelBuffer> sBuffer) // Used when making a cross process canvas
		: _m_width(width), _m_height(height), _m_screenBuffer(std::move(sBuffer))
	{}

	unsigned short PixelCanvas::GetWidth() const
	{
		return _m_width;
	}

	unsigned short PixelCanvas::GetHeight() const
	{
		return _m_height;
	}

	void PixelCanvas::SetPixel(unsigned int x, unsigned int y, CHAR_INFO s_pixel)
	{
		// if the index is it side the screen buffer return
		int index = y * _m_screenBuffer->m_bufferSize.X + x;
		if (index < 0 || index >= _m_screenBuffer->m_bufferSize.X * _m_screenBuffer->m_bufferSize.Y)
			return;

		_m_screenBuffer->GetBuffer()[index] = s_pixel;
	}

	void PixelCanvas::SetPixels(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest)
	{
		// pointer to the end of the screen buffer, calculated based on screen dimensions (width * height) aka "size".
		const CHAR_INFO* bufferEnd = _m_screenBuffer->GetBuffer() + _m_screenBuffer->m_size;

		// calculate the remaining space in the screen buffer starting from the destination pointer.
		const std::size_t remainingBufferSpace = bufferEnd - dest;

		// calculate the number of elements in the source range.
		const std::size_t sourceElementCount = srcEnd - srcStart;

		// determine the maximum number of elements that can be safely copied to the screen buffer.
		// this is the lesser of the remaining buffer space or the source element count.
		const std::size_t elementsToCopy = std::min<std::size_t>(remainingBufferSpace, sourceElementCount);

		// store the transparent character value used for comparison.
		constexpr wchar_t transparentChar = L'‎';

		// initialize a pointer to track the previous position in the destination buffer.
		CHAR_INFO* previousDestPosition = dest - 1;

		std::transform(srcStart, srcStart + elementsToCopy, dest,
			[&transparentChar, &previousDestPosition](const CHAR_INFO& currentElement)
			{
				previousDestPosition++;

				// if the current element's Unicode character is not the transparent character,
				// copy it to the destination buffer. Otherwise, use the value from the previous position in the buffer.
				// (if the char is a transparent char all that happens is we use the pixel at the postion that's already in the 
				// buffer instead of replacing it with a new one, this allows for non irregularly shaped sprites )
				return (currentElement.Char.UnicodeChar != transparentChar) ? currentElement : *previousDestPosition;
			}
		);
	}

	void PixelCanvas::FillCanvas(CHAR_INFO fillChar)
	{
		std::fill(_m_screenBuffer->GetBuffer(), _m_screenBuffer->GetBuffer() + _m_screenBuffer->m_size, fillChar);
	}

};
