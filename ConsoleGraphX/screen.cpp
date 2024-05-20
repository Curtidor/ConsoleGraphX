#include "screen.h"
#include <consoleapi3.h>
#include <windows.h>
#include <stdexcept>
#include <consoleapi2.h>
#include <utility>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <processenv.h>
#include <handleapi.h>
#include "screen_buffer.h"

namespace ConsoleGraphX_Interal
{
	Screen* Screen::_s_activeScreen = nullptr;

	Screen::Screen(short width, short height, short debugger_height, short fontWidth, short fontHeight)
		: _m_width(width), _m_height(height), _m_debuggerHeight(debugger_height), _m_pixelWidth(fontWidth), _m_pixelHeight(fontHeight)
	{
		_m_screenBuffer = new ConsoleGraphX_Interal::ScreenBuffer();

		_m_screenBuffer->hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		if (_m_screenBuffer->hConsole == INVALID_HANDLE_VALUE)
			throw std::runtime_error("Failed to get the console handle");
		
		if (!SetConsoleActiveScreenBuffer(_m_screenBuffer->hConsole))
			throw std::runtime_error("Failed set the screen buffer");
		
		short totalHeight = _m_height + _m_debuggerHeight;

		_m_bufferSizeB = _m_width * (_m_height + _m_debuggerHeight) * sizeof(CHAR_INFO);
		_m_bufferSize = _m_width * (_m_height + _m_debuggerHeight);

		// Set the buffer size and allocate memory for the buffer
		_m_screenBuffer->bufferSize = { _m_width, totalHeight };
		_m_screenBuffer->bufferCoord = { 0, 0 };
		_m_screenBuffer->buffer = new CHAR_INFO[_m_screenBuffer->bufferSize.X * _m_screenBuffer->bufferSize.Y];
		_m_screenBuffer->writePosition = { 0, 0, static_cast<short>(_m_screenBuffer->bufferSize.X - 1), static_cast<short>(_m_screenBuffer->bufferSize.Y - 1) };

		_m_screenBuffer->wHandle = GetConsoleWindow();
		if (_m_screenBuffer->wHandle == nullptr)
			throw std::runtime_error("no console window");

		SetConsoleScreenBufferSize(_m_screenBuffer->hConsole, _m_screenBuffer->bufferSize);

		// Set the console font size and window size
		SetConsoleFontSize(fontWidth, fontHeight);
		SetConsoleWindowSize(_m_screenBuffer->bufferSize.X, _m_screenBuffer->bufferSize.Y);

		MemFillScreen( 0 );

		Screen::_s_activeScreen = this;
	}

	Screen::~Screen()
	{
		delete _m_screenBuffer;
	}

	bool Screen::DrawScreen()
	{
		// Write the screen buffer to the console
		if (!WriteConsoleOutput(_m_screenBuffer->hConsole, _m_screenBuffer->buffer,
			_m_screenBuffer->bufferSize, _m_screenBuffer->bufferCoord, &_m_screenBuffer->writePosition))
		{
			return false;
		}
		return true;
	}

	void Screen::FillScreen(const CHAR_INFO& color)
	{
		std::fill(_m_screenBuffer->buffer, _m_screenBuffer->buffer + _m_bufferSize, color);
	}

	void Screen::MemFillScreen(int color)
	{
		memset(_m_screenBuffer->buffer, color, _m_bufferSizeB);
	}

	void Screen::RandomFillScreen()
	{
		for (int i = 0; i < Screen::GetHeight(); i++) {

			for (int j = 0; j < Screen::GetWidth(); j++) {

				CHAR_INFO ch = { s_blockPixel , Screen::RandomColor() };
				SetPixel(j, i, ch);

			}
		}
	}
	
	void Screen::SetPixel(int x, int y, CHAR_INFO& s_pixel)
	{
		if (s_pixel.Char.UnicodeChar == Screen::s_transparentPixel)
			return;

		// if the index is it side the screen buffer return
		int index = y * _m_screenBuffer->bufferSize.X + x;
		if (index < 0 || index >= _m_screenBuffer->bufferSize.X * (_m_screenBuffer->bufferSize.Y - _m_debuggerHeight))
			return;

		_m_screenBuffer->buffer[index] = s_pixel;
	}

	void Screen::SetPixel_A(int x, int y, CHAR_INFO& s_pixel)
	{
		Screen::_s_activeScreen->SetPixel(x, y, s_pixel);
	}

	void Screen::SetPixels(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest)
	{
		// pointer to the end of the screen buffer
		CHAR_INFO* bufferEnd = _m_screenBuffer->buffer + (_m_width * _m_height);

		// remaining space in the screen buffer
		std::size_t remainingBufferLength = bufferEnd - dest;
		// size of source elements
		std::size_t sourceLength = srcEnd - srcStart;

		// if there is less space in the screen buffer then n source elements use the buffer length, if 
		// there is more buffer space then n source elements use the source length so we only use what we need
		std::size_t maxLength = std::max<size_t>(0, std::min<std::size_t>(remainingBufferLength, sourceLength));

		wchar_t transparentChar = Screen::s_transparentPixel;

		CHAR_INFO* backgroundChar = dest - 1;

		std::transform(srcStart, srcStart + maxLength, dest, [&transparentChar, &backgroundChar](CHAR_INFO toCopyValue)
			{
				backgroundChar++;

				// If the Unicode character of toCopyValue is not equal to the transparentChar,
				// use the toCopyValue; otherwise, use the value from the previous position in the buffer (behind the transparentChar).
				return (toCopyValue.Char.UnicodeChar != transparentChar) ? toCopyValue : *backgroundChar;
			});
	}

	void Screen::SetPixels_A(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest)
	{
		Screen::_s_activeScreen->SetPixels(srcStart, srcEnd, dest);
	}

	void Screen::SetText(int x, int y, const std::string& text)
	{
		const int whiteTextColor = 0xf;
		int screenWidth = _m_screenBuffer->bufferSize.X;

		int index = y * screenWidth + x;

		if (index < 0 || index >= _m_screenBuffer->bufferSize.X * _m_screenBuffer->bufferSize.Y)
			return;

		for (int i = 0; i < text.size(); i++)
		{
			wchar_t c = text[i];
			_m_screenBuffer->buffer[index + i] = { c, whiteTextColor };
		}
	}

	void Screen::SetText_A(int x, int y, const std::string& text)
	{
		Screen::GetActiveScreen_A()->SetText(x, y, text);
	}

	bool Screen::SetConsoleFontSize(short width, short height)
	{
		// Set the console font size
		CONSOLE_FONT_INFOEX font = { sizeof(CONSOLE_FONT_INFOEX) };
		GetCurrentConsoleFontEx(_m_screenBuffer->hConsole, FALSE, &font);

		font.dwFontSize.X = width;
		font.dwFontSize.Y = height;
		if (!SetCurrentConsoleFontEx(_m_screenBuffer->hConsole, FALSE, &font)) {
			return false;
		}

		return true;
	}

	void Screen::SetConsoleWindowSize(short width, short height)
	{
		// Set the console window size
		SMALL_RECT rect = { 0, 0, width - 1, height - 1 };
		SetConsoleWindowInfo(_m_screenBuffer->hConsole, TRUE, &rect);
	}

	void Screen::SetCursorPosition(short x, short y)
	{
		// Set the cursor position in the console
		SetConsoleCursorPosition(_m_screenBuffer->hConsole, COORD{ x, y });
	}

	void Screen::SetConsoleName(const std::string& name)
	{
		if (_m_screenBuffer == nullptr)
			throw std::runtime_error("Screen has not been initialized.");

		SetConsoleTitleA(name.c_str());
	}

	void Screen::SetActiveScreen_A(Screen* screen) { Screen::_s_activeScreen = screen; }

	int Screen::GetWidth() const { return _m_width; }
	int Screen::GetHeight() const { return _m_height; }

	int Screen::GetPixelWidth() const { return _m_pixelWidth; }
	int Screen::GetPixelHeight() const { return _m_pixelHeight; }

	int Screen::GetWidth_A() { return Screen::_s_activeScreen->_m_width; }
	int Screen::GetHeight_A() { return Screen::_s_activeScreen->_m_height; }

	HWND Screen::GetConsoleWindowHandle() { return _m_screenBuffer->wHandle; }
	WORD Screen::RandomColor() {return rand() % 16;}
	
	Screen* Screen::GetActiveScreen_A() { return Screen::_s_activeScreen; }
	CHAR_INFO* Screen::GetActiveScreenBuffer_A() { return Screen::_s_activeScreen->_m_screenBuffer->buffer; }
};


