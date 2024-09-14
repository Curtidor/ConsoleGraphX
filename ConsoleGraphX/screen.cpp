#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "wincontypes.h"
#include "screen.h"
#include "screen_buffer.h"
#include <consoleapi2.h>
#include <consoleapi3.h>
#include <handleapi.h>
#include <processenv.h>
#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <utility>
#include <array>


namespace ConsoleGraphX_Internal
{
	Screen* Screen::_s_activeScreen = nullptr;

	Screen::Screen(short width, short height, short fontWidth, short fontHeight)
		: _m_width(width), _m_height(height), _m_pixelWidth(fontWidth), _m_pixelHeight(fontHeight)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		_m_screenBuffer = new ScreenBuffer(hConsole, width, _m_height);

		if (_m_screenBuffer->m_hConsole == INVALID_HANDLE_VALUE)
			throw std::runtime_error("Failed to get the console handle");

		SetConsoleScreenBufferSize(_m_screenBuffer->m_hConsole, _m_screenBuffer->m_bufferSize);

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
		if (!WriteConsoleOutput(_m_screenBuffer->m_hConsole, _m_screenBuffer->m_buffer,
			_m_screenBuffer->m_bufferSize, _m_screenBuffer->m_bufferCoord, &_m_screenBuffer->m_writePosition))
		{
			return false;
		}
		return true;
	}

	void Screen::FillScreen(const CHAR_INFO& color)
	{
		std::fill(_m_screenBuffer->m_buffer, _m_screenBuffer->m_buffer + (_m_width * _m_height), color);
	}

	void Screen::RandomFillScreen()
	{
		for (int i = 0; i < Screen::GetHeight(); i++) {

			for (int j = 0; j < Screen::GetWidth(); j++) {

				SetPixel(j, i, { s_pixel , Screen::RandomColor() });

			}
		}
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

		_m_screenBuffer->m_buffer[index] = s_pixel;
	}

	void Screen::SetPixel_A(int x, int y, CHAR_INFO s_pixel)
	{
		if (s_pixel.Char.UnicodeChar == Screen::s_transparentPixel)
			return;

		Screen::_s_activeScreen->SetPixel(x, y, s_pixel);
	}

	void Screen::SetPixels(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest)
	{
		// pointer to the end of the screen buffer, calculated based on screen dimensions (width * height, (size)).
		const CHAR_INFO* bufferEnd = _m_screenBuffer->m_buffer + _m_screenBuffer->m_size;

		// calculate the remaining space in the screen buffer starting from the destination pointer.
		const std::size_t remainingBufferSpace = bufferEnd - dest;

		// calculate the number of elements in the source range.
		const std::size_t sourceElementCount = srcEnd - srcStart;

		// determine the maximum number of elements that can be safely copied to the screen buffer.
		// this is the lesser of the remaining buffer space or the source element count.
		const std::size_t elementsToCopy = std::min<std::size_t>(remainingBufferSpace, sourceElementCount);

		// store the transparent character value used for comparison.
		const wchar_t transparentChar = Screen::s_transparentPixel;

		// initialize a pointer to track the previous position in the destination buffer.
		CHAR_INFO* previousDestPosition = dest - 1;

		// use std::transform to copy and potentially modify the elements from the source range to the destination buffer.
		std::transform(srcStart, srcStart + elementsToCopy, dest,
			[&transparentChar, &previousDestPosition](const CHAR_INFO& currentElement)
			{
				// move the pointer to the current position in the destination buffer.
				previousDestPosition++;

				// if the current element's Unicode character is not the transparent character,
				// copy it to the destination buffer. Otherwise, use the value from the previous position in the buffer.
				return (currentElement.Char.UnicodeChar != transparentChar) ? currentElement : *previousDestPosition;
			}
		);
	}

	void Screen::SetPixels_A(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest)
	{
		Screen::_s_activeScreen->SetPixels(srcStart, srcEnd, dest);
	}

	void Screen::SetText(int x, int y, const std::string& text)
	{
		const int white_text_color = 0xf;
		int screenWidth = _m_screenBuffer->m_bufferSize.X;

		int index = y * screenWidth + x;

		if (index < 0 || index >= _m_screenBuffer->m_bufferSize.X * _m_screenBuffer->m_bufferSize.Y)
			return;

		for (int i = 0; i < text.size(); i++)
		{
			wchar_t c = text[i];
			_m_screenBuffer->m_buffer[index + i] = { c, white_text_color };
		}
	}

	void Screen::SetText_A(int x, int y, const std::string& text)
	{
		Screen::GetActiveScreen_A()->SetText(x, y, text);
	}

	/// <summary>
	/// sets the font(pixel) size of the console
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <returns></returns>
	bool Screen::SetConsoleFontSize(short width, short height)
	{
		// Set the console font size
		CONSOLE_FONT_INFOEX font = { sizeof(CONSOLE_FONT_INFOEX) };
		GetCurrentConsoleFontEx(_m_screenBuffer->m_hConsole, FALSE, &font);

		font.dwFontSize.X = width;
		font.dwFontSize.Y = height;
		if (!SetCurrentConsoleFontEx(_m_screenBuffer->m_hConsole, FALSE, &font)) {
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
		SetConsoleWindowInfo(_m_screenBuffer->m_hConsole, TRUE, &rect);
	}


	/// <summary>
	/// Set the position of the console cursor
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	void Screen::SetCursorPosition(short x, short y)
	{
		// Set the cursor position in the console
		SetConsoleCursorPosition(_m_screenBuffer->m_hConsole, COORD{ x, y });
	}

	void Screen::SetConsoleName(const std::string& name)
	{
		if (_m_screenBuffer == nullptr)
			throw std::runtime_error("Screen has not been initialized.");

		SetConsoleTitleA(name.c_str());
	}

	/// <summary>
	/// Get the width of the screen
	/// </summary>
	/// <returns></returns>
	int Screen::GetWidth() const { return _m_width; }

	/// <summary>
	/// Get the height of the screen
	/// </summary>
	/// <returns></returns>
	int Screen::GetHeight() const { return _m_height; }

	/// <summary>
	/// Get the width of a pixel
	/// </summary>
	/// <returns></returns>
	int Screen::GetPixelWidth() const { return _m_pixelWidth; }

	/// <summary>
	/// Get the height of a pixel
	/// </summary>
	/// <returns></returns>
	int Screen::GetPixelHeight() const { return _m_pixelHeight; }

	void Screen::SetPalletColors(const std::array<RGB_CGX, 16>& colors) 
	{
		CONSOLE_SCREEN_BUFFER_INFOEX consoleInfo{};
		consoleInfo.cbSize = sizeof(consoleInfo);

		GetConsoleScreenBufferInfoEx(_s_activeScreen->_m_screenBuffer->m_hConsole, &consoleInfo);

		for (size_t i = 0; i < colors.size(); i++) 
		{
			consoleInfo.ColorTable[i] = RGB(colors[i].r, colors[i].g, colors[i].b);
		}

		SetConsoleScreenBufferInfoEx(_s_activeScreen->_m_screenBuffer->m_hConsole, &consoleInfo);
	}


	int Screen::GetWidth_A() { return Screen::_s_activeScreen->_m_width; }
	int Screen::GetHeight_A() { return Screen::_s_activeScreen->_m_height; }

	WORD Screen::RandomColor()
	{
		return rand() % 16;
	}

	Screen* Screen::GetActiveScreen_A() { return Screen::_s_activeScreen; }
	void Screen::SetActiveScreen_A(Screen* screen) { Screen::_s_activeScreen = screen; }
	CHAR_INFO* Screen::GetActiveScreenBuffer_A() { return Screen::_s_activeScreen->_m_screenBuffer->m_buffer; }
};
