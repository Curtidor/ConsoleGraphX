#include "screen.h"

Screen* Screen::_s_activeScreen = nullptr;

Screen::Screen(short width, short height, short debugger_height, short fontWidth, short fontHeight)
	: _m_width(width), _m_height(height), _m_debuggerHeight(debugger_height), _m_pixelWidth(fontWidth), _m_pixelHeight(fontHeight)
{
	this->_m_screenBuffer = new ScreenBuffer();

	// Get the console handle
	this->_m_screenBuffer->hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (this->_m_screenBuffer->hConsole == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Failed to get the console handle");

	short total_height = _m_height + _m_debuggerHeight;

	// Set the buffer size and allocate memory for the buffer
	this->_m_screenBuffer->bufferSize = { _m_width, total_height };
	this->_m_screenBuffer->bufferCoord = { 0, 0 };
	this->_m_screenBuffer->buffer = new CHAR_INFO[_m_screenBuffer->bufferSize.X * _m_screenBuffer->bufferSize.Y];
	this->_m_screenBuffer->writePosition = { 0, 0, (short)(_m_screenBuffer->bufferSize.X - 1), (short)(_m_screenBuffer->bufferSize.Y - 1) };

	this->_m_screenBuffer->wHandle = GetConsoleWindow();
	if (this->_m_screenBuffer->wHandle == NULL)
		throw new std::runtime_error("no console window");

	// Set the console screen buffer size
	COORD totalBufferSize = { _m_width , total_height };
	SetConsoleScreenBufferSize(_m_screenBuffer->hConsole, totalBufferSize);

	// Set the console font size and window size
	SetConsoleFontSize(fontWidth, fontHeight);
	SetConsoleWindowSize(_m_width, total_height);

	FillScreen({ s_pixel , 0 });

	Screen::_s_activeScreen = this;
}

Screen::~Screen()
{
	delete this->_m_screenBuffer;
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
	std::fill(_m_screenBuffer->buffer, _m_screenBuffer->buffer + (_m_width * (_m_height + _m_debuggerHeight)), color);
}

void Screen::RandomFillScreen()
{
	for (int i = 0; i < Screen::GetHeight(); i++) {

		for (int j = 0; j < Screen::GetWidth(); j++) {

			this->SetPixel(j, i, { s_pixel , Screen::RandomColor() });

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
	int index = y * _m_screenBuffer->bufferSize.X + x;
	if (index < 0 || index >= _m_screenBuffer->bufferSize.X * (_m_screenBuffer->bufferSize.Y - _m_debuggerHeight))
		return;

	_m_screenBuffer->buffer[index] = s_pixel;
}

void Screen::SetPixel_A(int x, int y, CHAR_INFO s_pixel)
{
	if (s_pixel.Char.UnicodeChar == Screen::s_transparentPixel)
		return;

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
	
	wchar_t noDrawChar = Screen::s_transparentPixel;

	CHAR_INFO* destPrer = dest - 1;
	std::transform(srcStart, srcStart + maxLength, dest, [noDrawChar, &dest](CHAR_INFO toCopyValue) 
		{	
			dest++;
			return (toCopyValue.Char.UnicodeChar != noDrawChar) ? toCopyValue : *dest;
		});
	
	//std::copy(srcStart, srcStart + maxLength, dest);
}

void Screen::SetPixels_A(CHAR_INFO* srcStart, CHAR_INFO* srcEnd, CHAR_INFO* dest)
{
	Screen::_s_activeScreen->SetPixels(srcStart, srcEnd, dest);
}

void Screen::SetText(int x, int y, const std::string& text)
{
	const int white_text_color = 0xf;
	int screenWidth = _m_screenBuffer->bufferSize.X;

	int index = y * screenWidth + x;

	if (index < 0 || index >= _m_screenBuffer->bufferSize.X * _m_screenBuffer->bufferSize.Y)
		return;

	for (int i = 0; i < text.size(); i++)
	{
		wchar_t c = text[i];
		this->_m_screenBuffer->buffer[index + i] = { c, white_text_color };
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
	GetCurrentConsoleFontEx(_m_screenBuffer->hConsole, FALSE, &font);

	font.dwFontSize.X = width;
	font.dwFontSize.Y = height;
	if (!SetCurrentConsoleFontEx(_m_screenBuffer->hConsole, FALSE, &font)) {
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
	SetConsoleWindowInfo(_m_screenBuffer->hConsole, TRUE, &rect);
}


/// <summary>
/// Set the position of the console cursor
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
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



/// <summary>
/// Get the width of the screen
/// </summary>
/// <returns></returns>
int Screen::GetWidth() { return this->_m_width; }

/// <summary>
/// Get the height of the screen
/// </summary>
/// <returns></returns>
int Screen::GetHeight() { return this->_m_height; }

/// <summary>
/// Get the width of a pixel
/// </summary>
/// <returns></returns>
int Screen::GetPixelWidth() { return this->_m_pixelWidth; }

/// <summary>
/// Get the height of a pixel
/// </summary>
/// <returns></returns>
int Screen::GetPixelHeight() { return this->_m_pixelHeight; }

int Screen::GetWidth_A() { return Screen::_s_activeScreen->_m_width; }
int Screen::GetHeight_A() { return Screen::_s_activeScreen->_m_height; }

HWND Screen::GetConsoleWindowHandle() { return this->_m_screenBuffer->wHandle; }

WORD Screen::RandomColor()
{
	return rand() % 16;
}

Screen* Screen::GetActiveScreen_A() { return Screen::_s_activeScreen; }
void Screen::SetActiveScreen_A(Screen* screen) { Screen::_s_activeScreen = screen; }
CHAR_INFO* Screen::GetActiveScreenBuffer_A() { return Screen::_s_activeScreen->_m_screenBuffer->buffer; }
