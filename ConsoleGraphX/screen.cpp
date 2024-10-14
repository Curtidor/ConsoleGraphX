#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <consoleapi2.h>
#include <consoleapi3.h>
#include <handleapi.h>
#include <processenv.h>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>
#include <array>
#include <wincontypes.h>
#include "screen.h"
#include "screen_buffer.h"
#include "color.h"
#include "palette.h"
#include "screen_buffer_shared.h"


namespace ConsoleGraphX_Internal
{
	Screen* Screen::_s_activeScreen = nullptr;

	Screen::Screen(short width, short height, short fontWidth, short fontHeight, ScreenBufferShared* screenBuffer)
		: _m_width(width), _m_height(height), _m_pixelWidth(fontWidth), _m_pixelHeight(fontHeight), _m_screenBuffer(screenBuffer)
	{
		SetConsoleScreenBufferSize(_m_screenBuffer->GetConsoleHandle(), _m_screenBuffer->GetBufferSize());

		SetConsoleFontSize(fontWidth, fontHeight);
		SetConsoleWindowSize(_m_width, _m_height);

		FillScreen({ s_pixel , 0 });

		Screen::_s_activeScreen = this;
	}


	Screen::Screen(short width, short height, short fontWidth, short fontHeight)
		: _m_width(width), _m_height(height), _m_pixelWidth(fontWidth), _m_pixelHeight(fontHeight)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		_m_screenBuffer = new ScreenBuffer(hConsole, width, _m_height);

		if (_m_screenBuffer->GetConsoleHandle() == INVALID_HANDLE_VALUE)
			throw std::runtime_error("Failed to get the console handle");

		SetConsoleScreenBufferSize(_m_screenBuffer->GetConsoleHandle(), _m_screenBuffer->GetBufferSize());

		SetConsoleFontSize(fontWidth, fontHeight);
		SetConsoleWindowSize(_m_width, _m_height);

		FillScreen({ s_pixel , 0 });

		Screen::_s_activeScreen = this;
	}

	Screen::~Screen()
	{
		delete _m_screenBuffer;
	}

	bool Screen::DrawScreen()
	{
		// Write the screen buffer to the console
		if (!WriteConsoleOutput(_m_screenBuffer->GetConsoleHandle(), _m_screenBuffer->GetBuffer(),
			_m_screenBuffer->GetBufferSize(), _m_screenBuffer->m_bufferCoord, &_m_screenBuffer->m_writePosition))
		{
			return false;
		}
		return true;
	}

	void Screen::FillScreen(const CHAR_INFO& color)
	{
		std::fill(_m_screenBuffer->GetBuffer(), _m_screenBuffer->GetBuffer() + _m_screenBuffer->m_size, color);
	}

	/// <summary>
	/// Set the pixel at the specified coordinates in the screen buffer
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="pixel"></param>
	void Screen::SetPixel(int x, int y, CHAR_INFO s_pixel)
	{
		// if the index is it side the screen buffer return
		int index = y * _m_screenBuffer->m_bufferSize.X + x;
		if (index < 0 || index >= _m_screenBuffer->m_bufferSize.X * _m_screenBuffer->m_bufferSize.Y)
			return;

		_m_screenBuffer->GetBuffer()[index] = s_pixel;
	}

	void Screen::SetPixel_A(int x, int y, CHAR_INFO s_pixel)
	{
		if (s_pixel.Char.UnicodeChar == Screen::s_transparentPixel)
			return;

		Screen::_s_activeScreen->SetPixel(x, y, s_pixel);
	}

	void Screen::SetPixels(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest)
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
		const wchar_t& transparentChar = Screen::s_transparentPixel;

		// initialize a pointer to track the previous position in the destination buffer.
		CHAR_INFO* previousDestPosition = dest - 1;

		std::transform(srcStart, srcStart + elementsToCopy, dest,
			[&transparentChar, &previousDestPosition](const CHAR_INFO& currentElement)
			{
				previousDestPosition++;

				// if the current element's Unicode character is not the transparent character,
				// copy it to the destination buffer. Otherwise, use the value from the previous position in the buffer.
				// (if the char is a transparent char all that happens is we use the pixel at the postion thats already in the 
				// buffer instead of replacing it with a new one, this allows for non irregularly shaped sprites )
				return (currentElement.Char.UnicodeChar != transparentChar) ? currentElement : *previousDestPosition;
			}
		);
	}

	void Screen::SetPixels_A(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest)
	{
		Screen::_s_activeScreen->SetPixels(srcStart, srcEnd, dest);
	}

	/// sets the font(pixel) size of the console
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <returns></returns>
	bool Screen::SetConsoleFontSize(short width, short height)
	{
		// Set the console font size
		CONSOLE_FONT_INFOEX font = { sizeof(CONSOLE_FONT_INFOEX) };
		GetCurrentConsoleFontEx(_m_screenBuffer->GetConsoleHandle(), FALSE, &font);

		font.dwFontSize.X = width;
		font.dwFontSize.Y = height;
		if (!SetCurrentConsoleFontEx(_m_screenBuffer->GetConsoleHandle(), FALSE, &font)) {
			return false;
		}

		return true;
	}

	/// <summary>
	/// Set the size of the window
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void Screen::SetConsoleWindowSize(short width, short height)
	{
		// Set the console window size
		SMALL_RECT rect = { 0, 0, width - 1, height - 1 };
		SetConsoleWindowInfo(_m_screenBuffer->GetConsoleHandle(), TRUE, &rect);
	}


	/// <summary>
	/// Set the position of the console cursor
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	void Screen::SetCursorPosition(short x, short y)
	{
		// Set the cursor position in the console
		SetConsoleCursorPosition(_m_screenBuffer->GetConsoleHandle(), COORD{x, y});
	}

	void Screen::SetConsoleName(const std::string& name)
	{
		if (_m_screenBuffer == nullptr)
			throw std::runtime_error("Screen has not been initialized.");

		SetConsoleTitleA(name.c_str());
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

	/// <summary>
	/// Get the width of the screen
	/// </summary>
	/// <returns></returns>
	int Screen::GetWidth() const 
	{ 
		return _m_width; 
	}

	/// <summary>
	/// Get the height of the screen
	/// </summary>
	/// <returns></returns>
	int Screen::GetHeight() const 
	{ 
		return _m_height; 
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
