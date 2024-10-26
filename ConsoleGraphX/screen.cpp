#include "PCH_CGX.h"
#include <handleapi.h>
#include "screen.h"
#include "WinCore.h"
#include "pixel_buffer.h"
#include "color.h"
#include "palette.h"


namespace ConsoleGraphX_Internal
{
	Screen::Screen(unsigned short width, unsigned short height, unsigned short fontWidth, unsigned short fontHeight, std::unique_ptr<PixelBuffer> sBuffer)
		: PixelCanvas(width, height, std::move(sBuffer)),
		_m_pixelWidth(fontWidth), _m_pixelHeight(fontHeight)
	{}


	Screen::Screen(unsigned short width, unsigned short height, unsigned short fontWidth, unsigned short fontHeight)
		: PixelCanvas(width, height),
		 _m_pixelWidth(fontWidth), _m_pixelHeight(fontHeight)
	{
		Initialize();
	}

	bool Screen::DrawScreen()
	{
		if (!WriteConsoleOutput(_m_screenBuffer->GetConsoleHandle(), _m_screenBuffer->GetBuffer(),
			_m_screenBuffer->GetBufferSize(), _m_screenBuffer->m_bufferCoord, &_m_screenBuffer->m_writePosition))
		{
			return false;
		}
		return true;
	}

	void Screen::SetPixel_A(int x, int y, CHAR_INFO s_pixel)
	{
		if (s_pixel.Char.UnicodeChar == Screen::s_transparentPixel)
			return;

		Screen::_s_activeScreen->SetPixel(x, y, s_pixel);
	}


	void Screen::SetPixels_A(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest)
	{
		Screen::_s_activeScreen->SetPixels(srcStart, srcEnd, dest);
	}

	void Screen::Initialize()
	{
		SetConsoleScreenBufferSize(_m_screenBuffer->GetConsoleHandle(), _m_screenBuffer->GetBufferSize());

		SetConsoleFontSizeWC(_m_screenBuffer->GetConsoleHandle(), _m_pixelWidth, _m_pixelHeight);
		SetConsoleWindowSizeWC(_m_screenBuffer->GetConsoleHandle(), _m_width, _m_height);

		FillCanvas({ s_pixel , 0 });

		//Screen::_s_activeScreen = this;
	}

	bool Screen::WriteText(const std::string& text, short x, short y)
	{
		PixelBuffer* buffer = _m_screenBuffer.get();
		int screenWidth = buffer->m_bufferSize.X;
		int screenHeight = buffer->m_bufferSize.Y;

		bool hadOverlap = false;
		for (int i = 0; i < text.size(); i++)
		{
			// check if the character would go beyond the right edge of the screen
			if (x + i >= screenWidth)
			{
				x = 0; 
				y += 1;

				return true;
			}

			// check if we reached the end of the screen buffer vertically
			if (y >= screenHeight)
			{
				y = 0; // start back at the top if it overflows
				return true;
			}

			int index = y * screenWidth + x + i;
			buffer->GetBuffer()[index].Char.UnicodeChar = text[i];
			buffer->GetBuffer()[index].Attributes = 3;
		}

		return hadOverlap;
	}



	void Screen::WriteTextColor(CHAR_INFO* text, short x, short y)
	{
		ConsoleGraphX_Internal::PixelBuffer* pBuffer = _m_screenBuffer.get();

		SMALL_RECT writeRegion = { x, y, pBuffer->m_writePosition.Right, pBuffer->m_writePosition.Top };

		WriteConsoleOutputA(pBuffer->GetConsoleHandle(), text, pBuffer->m_bufferSize, pBuffer->m_bufferCoord, &writeRegion);
	}

	/// <summary>
	/// Get the width of a pixel
	/// </summary>
	/// <returns></returns>
	int Screen::GetPixelWidth() const
	{ 
		return _m_pixelWidth; 
	}

	/// <summary>
	/// Get the height of a pixel
	/// </summary>
	/// <returns></returns>
	int Screen::GetPixelHeight() const 
	{ 
		return _m_pixelHeight; 
	}

	CHAR_INFO* Screen::GetScreenBuffer()
	{
		return _m_screenBuffer.get()->GetBuffer();
	}

	int Screen::GetWidth_A() 
	{
		return Screen::_s_activeScreen->_m_width; 
	}

	int Screen::GetHeight_A()
	{ 
		return Screen::_s_activeScreen->_m_height; 
	}

	void Screen::SetPalletColors_A(std::array<ConsoleGraphX::Color_CGX, 16>& paletteColors) 
	{
		CONSOLE_SCREEN_BUFFER_INFOEX consoleInfo{};
		consoleInfo.cbSize = sizeof(consoleInfo);

		GetConsoleScreenBufferInfoEx(_s_activeScreen->_m_screenBuffer->GetConsoleHandle(), &consoleInfo);

		for (size_t i = 0; i < paletteColors.size(); i++) 
		{
			consoleInfo.ColorTable[i] = RGB(paletteColors[i].r, paletteColors[i].g, paletteColors[i].b);
		}

		SetConsoleScreenBufferInfoEx(_s_activeScreen->_m_screenBuffer->GetConsoleHandle(), &consoleInfo);
	}

	void Screen::SetPalletColors_A(ConsoleGraphX::Palette& paletteColors)
	{
		CONSOLE_SCREEN_BUFFER_INFOEX consoleInfo{};
		consoleInfo.cbSize = sizeof(consoleInfo);

		GetConsoleScreenBufferInfoEx(_s_activeScreen->_m_screenBuffer->GetConsoleHandle(), &consoleInfo);

		const std::array<ConsoleGraphX::Color_CGX, 16>& colors = paletteColors.GetColors();
		for (size_t i = 0; i < colors.size(); i++)
		{
			consoleInfo.ColorTable[i] = RGB(colors[i].r, colors[i].g, colors[i].b);
		}

		SetConsoleScreenBufferInfoEx(_s_activeScreen->_m_screenBuffer->GetConsoleHandle(), &consoleInfo);
	}

	void Screen::SetPalletColor_A(const ConsoleGraphX::Color_CGX& color, int index)
	{
		if (index < 0 || index > 15)
		{
			throw std::runtime_error("Index must be, between 0-15");
		}

		CONSOLE_SCREEN_BUFFER_INFOEX consoleInfo{};
		consoleInfo.cbSize = sizeof(consoleInfo);

		GetConsoleScreenBufferInfoEx(_s_activeScreen->_m_screenBuffer->GetConsoleHandle(), &consoleInfo);

		consoleInfo.ColorTable[index] = RGB(color.r, color.g, color.b);

		SetConsoleScreenBufferInfoEx(_s_activeScreen->_m_screenBuffer->GetConsoleHandle(), &consoleInfo);
	}

	void Screen::SetActiveScreen_A(Screen* screen) 
	{
		Screen::_s_activeScreen = screen; 
	}

	Screen* Screen::GetActiveScreen_A() 
	{
		return Screen::_s_activeScreen; 
	}

	CHAR_INFO* Screen::GetActiveScreenBuffer_A() 
	{ 
		return Screen::_s_activeScreen->_m_screenBuffer->GetBuffer(); 
	}
};
