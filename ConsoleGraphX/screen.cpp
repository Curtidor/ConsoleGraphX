#include "screen.h"

Screen* Screen::s_active_screen = nullptr;

Screen::Screen(short width, short height, short debugger_height, short fontWidth, short fontHeight)
	: m_width(width), m_height(height), m_debugger_height(debugger_height), m_pixel_width(fontWidth), m_pixel_height(fontHeight)
{
	this->m_screenBuffer = new ScreenBuffer();

	// Get the console handle
	this->m_screenBuffer->hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (this->m_screenBuffer->hConsole == INVALID_HANDLE_VALUE)
		throw std::runtime_error("Failed to get the console handle");


	short total_height = m_height + m_debugger_height;

	// Set the buffer size and allocate memory for the buffer
	this->m_screenBuffer->bufferSize = { m_width, total_height };
	this->m_screenBuffer->buffer = new CHAR_INFO[m_screenBuffer->bufferSize.X * m_screenBuffer->bufferSize.Y];
	this->m_screenBuffer->bufferCoord = { 0, 0 };
	this->m_screenBuffer->writePosition = { 0, 0, (short)(m_screenBuffer->bufferSize.X - 1), (short)(m_screenBuffer->bufferSize.Y - 1) };

	this->m_screenBuffer->wHandle = GetConsoleWindow();

	// Set the console screen buffer size
	COORD totalBufferSize = { m_width , total_height };
	SetConsoleScreenBufferSize(m_screenBuffer->hConsole, totalBufferSize);

	// Set the console font size and window size
	SetConsoleFontSize(fontWidth, fontHeight);
	SetConsoleWindowSize(m_width, total_height);

	FillScreen({ pixel , 0 });

	Screen::s_active_screen = this;
}

Screen::~Screen()
{
	delete this->m_screenBuffer;
}

bool Screen::DrawScreen()
{
	// Write the screen buffer to the console
	if (!WriteConsoleOutput(m_screenBuffer->hConsole, m_screenBuffer->buffer,
		m_screenBuffer->bufferSize, m_screenBuffer->bufferCoord, &m_screenBuffer->writePosition))
	{
		return false;
	}
	return true;
}

void Screen::FillScreen(const CHAR_INFO& color)
{
	std::fill(m_screenBuffer->buffer, m_screenBuffer->buffer + (m_width * (m_height + m_debugger_height)), color);
}

void Screen::RandomFillScreen()
{
	for (int i = 0; i < Screen::GetHeight(); i++) {

		for (int j = 0; j < Screen::GetWidth(); j++) {

			this->SetPixel(j, i, { pixel , Screen::RandomColor() });

		}
	}
}
/// <summary>
/// Set the pixel at the specified coordinates in the screen buffer
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="pixel"></param>
void Screen::SetPixel(int x, int y, CHAR_INFO pixel)
{
	// if the index is it side the screen buffer return
	int index = y * m_screenBuffer->bufferSize.X + x;
	if (index < 0 || index >= m_screenBuffer->bufferSize.X * (m_screenBuffer->bufferSize.Y - m_debugger_height))
		return;

	m_screenBuffer->buffer[index] = pixel;
}

void Screen::SetPixel_A(int x, int y, CHAR_INFO pixel)
{
	Screen::s_active_screen->SetPixel(x, y, pixel);
}

void Screen::SetPixels(CHAR_INFO* src_start, CHAR_INFO* src_end, CHAR_INFO* dest)
{
	// pointer to the end of the screen buffer
	CHAR_INFO* buffer_end = m_screenBuffer->buffer + (m_width * m_height);

	// remaining space in the screen buffer
	std::size_t buffer_length = buffer_end - dest;
	// remaining amount of source elements
	std::size_t source_length = src_end - src_start;

	// if there is less space in the screen buffer then n source elements use the buffer length, if 
	// there is more buffer space then n source elements use the source length so we only use what we need
	//NOTE: we use max and min instead of if else to avoid CPU branching
	std::size_t max_length = std::max<size_t>(0, std::min<std::size_t>(buffer_length, source_length));

	std::copy(src_start, src_start + max_length, dest);
}

void Screen::SetPixels_A(CHAR_INFO* src_start, CHAR_INFO* src_end, CHAR_INFO* dest)
{
	Screen::s_active_screen->SetPixels(src_start, src_end, dest);
}

void Screen::SetText(int x, int y, const std::string& text)
{
	const int white_text_color = 0xf;
	int screenWidth = m_screenBuffer->bufferSize.X;

	int index = y * screenWidth + x;

	if (index < 0 || index >= m_screenBuffer->bufferSize.X * m_screenBuffer->bufferSize.Y)
		return;

	for (int i = 0; i < text.size(); i++)
	{
		wchar_t c = text[i];
		this->m_screenBuffer->buffer[index + i] = { c, white_text_color };
	}
}


void Screen::SetText_A(int x, int y, const std::string& text)
{
	Screen::GetActiveScreen()->SetText(x, y, text);
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
	GetCurrentConsoleFontEx(m_screenBuffer->hConsole, FALSE, &font);

	font.dwFontSize.X = width;
	font.dwFontSize.Y = height;
	if (!SetCurrentConsoleFontEx(m_screenBuffer->hConsole, FALSE, &font)) {
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
	SetConsoleWindowInfo(m_screenBuffer->hConsole, TRUE, &rect);
}


/// <summary>
/// Set the position of the console cursor
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
void Screen::SetCursorPosition(short x, short y)
{
	// Set the cursor position in the console
	SetConsoleCursorPosition(m_screenBuffer->hConsole, COORD{ x, y });
}

void Screen::SetConsoleName(const std::string& name)
{
	if (m_screenBuffer == nullptr)
		throw std::runtime_error("Screen has not been initialized.");

	SetConsoleTitleA(name.c_str());
}



/// <summary>
/// Get the width of the screen
/// </summary>
/// <returns></returns>
int Screen::GetWidth() { return this->m_width; }

/// <summary>
/// Get the height of the screen
/// </summary>
/// <returns></returns>
int Screen::GetHeight() { return this->m_height; }

/// <summary>
/// Get the width of a pixel
/// </summary>
/// <returns></returns>
int Screen::GetPixelWidth() { return this->m_pixel_width; }

/// <summary>
/// Get the height of a pixel
/// </summary>
/// <returns></returns>
int Screen::GetPixelHeight() { return this->m_pixel_height; }

int Screen::GetWidth_A() { return Screen::s_active_screen->m_width; }
int Screen::GetHeight_A() { return Screen::s_active_screen->m_height; }

HWND Screen::GetConsoleWindowHandle() { return this->m_screenBuffer->wHandle; }

WORD Screen::RandomColor()
{
	return rand() % 16;
}

Screen* Screen::GetActiveScreen() { return Screen::s_active_screen; }
CHAR_INFO* Screen::GetBuffer() { return Screen::s_active_screen->m_screenBuffer->buffer; }
