#include "PCH_CGX.h"
#include <Windows.h>
#include <handleapi.h>
#include <commctrl.h>
#include "Engine\Graphics\ScreenGraphics\screen.h"
#include "WinCore.h"
#include "Engine\Graphics\ScreenGraphics\pixel_buffer.h"
#include "Engine\Graphics\color.h"
#include "Engine\Graphics\palette.h"
#include "Engine/termlog.h"
#include "Engine/Core//Logger/logger_manager.h"



namespace ConsoleGraphX_Internal
{
	Screen::Screen(uint16_t width, uint16_t height, uint16_t fontWidth, uint16_t fontHeight, std::unique_ptr<PixelBuffer> sBuffer)
		: PixelCanvas(width, height, std::move(sBuffer)),
		_m_pixelWidth(fontWidth), _m_pixelHeight(fontHeight)
	{}


	Screen::Screen(uint16_t width, uint16_t height, uint16_t fontWidth, uint16_t fontHeight)
		: PixelCanvas(width, height),
		 _m_pixelWidth(fontWidth), _m_pixelHeight(fontHeight)
	{
		SetConsoleOutputCP(CP_UTF8);
	}

	bool Screen::DrawScreen()
	{
		/*
		 * OPTIMIZATION IDEA:
		 * Instead of redrawing the entire screen buffer every frame, consider dynamically adjusting
		 * the size of the rectangular region (`m_writePosition`) to only cover areas that have changed.
		 *
		 * Benefits:
		 * - Reduces the amount of data sent to the console, improving performance.
		 * - Minimizes unnecessary operations for static or mostly unchanged screens.
		 *
		 * Implementation Notes:
		 * - Track changes to the screen buffer (e.g., using a dirty region system or change flags).
		 * - Update only the affected region instead of the entire screen.
		 * - Handle edge cases where multiple regions are updated (merge regions or handle them sequentially).
		 */
		if (!WriteConsoleOutput(
			_m_screenBuffer->GetConsoleHandle(),
			_m_screenBuffer->GetBuffer(),
			_m_screenBuffer->GetBufferSize(),
			_m_screenBuffer->m_bufferCoord,
			&_m_screenBuffer->m_writePosition))
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

	// Returns how many characters were actually written.
	const bool Screen::WriteText(const std::string& text, uint16_t x, uint16_t y) const
	{
		PixelBuffer* buffer = _m_screenBuffer.get();
		CHAR_INFO* bufferData = buffer->GetBuffer();

		const int screenWidth = buffer->m_bufferSize.X;
		const int screenHeight = buffer->m_bufferSize.Y;

		if (screenWidth <= 0 || screenHeight <= 0)
		{
			return false;
		}
		if (x >= screenWidth)
		{
			x = 0;
			y += 1;
			// wrap to next line
		}
		if (y >= screenHeight)
		{
			return false; // nothing visible
		}
		

		size_t written = 0;
		size_t remaining = text.size();
		size_t textIdx = 0;

		// Helper: clear an entire row starting at colStart
		auto clearRowRange = [&](int row, int colStart) 
			{
				int base = row * screenWidth;
				for (int i = colStart; i < screenWidth; ++i) 
				{
					bufferData[base + i].Char.UnicodeChar = Screen::s_transparentPixel;
					bufferData[base + i].Attributes = 0; 
				}
			};

		// helper: write up to n chars on a row starting at colStart
		auto writeRowRange = [&](int row, int colStart, size_t n) 
			{
				int base = row * screenWidth + colStart;
				for (size_t i = 0; i < n; ++i) 
				{
					bufferData[base + i].Char.UnicodeChar = text[textIdx + i];
					bufferData[base + i].Attributes = 3;
				}
			written += n;
			remaining -= n;
			textIdx += n;
			};

		// first row (may be partial)
		{
			int capacity = screenWidth - static_cast<int>(x);
			size_t n = std::min<size_t>(remaining, static_cast<size_t>(capacity));

			// always clear from x to end of row so old chars don't remain
			clearRowRange(y, x);

			// write chars if we have them
			if (n > 0) 
			{
				writeRowRange(y, x, n);
			}
		}

		// subsequent full-width rows 
		int row = y + 1;
		while (remaining > 0 && row < screenHeight) 
		{
			size_t n = std::min<size_t>(remaining, static_cast<size_t>(screenWidth));

			// always clear the full row
			clearRowRange(row, 0);

			if (n > 0) 
			{
				writeRowRange(row, 0, n);
			}

			++row;
		}

		// return false if we ran out of screen space before text
		return (remaining == 0);
	}



	void Screen::WriteTextColor(CHAR_INFO* text, uint16_t x, uint16_t y)
	{
		ConsoleGraphX_Internal::PixelBuffer* pBuffer = _m_screenBuffer.get();

		SMALL_RECT writeRegion = { static_cast<short>(x), static_cast<short>(y), pBuffer->m_writePosition.Right, pBuffer->m_writePosition.Top };

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

	bool Screen::SetNewScreenSize(uint16_t width, uint16_t height, uint16_t fontWidth, uint16_t fontHeight)
	{
		// We set the console window size to 1x1 because if we try to set the console buffer size to dimensions smaller than 
		// the current window size, the operation will fail. This is due to a restriction in the Windows Console API, which requires
		// that the buffer size must always be at least as large as the window size.
		// By temporarily shrinking the window to its smallest possible size, we can freely adjust the buffer dimensions to our desired
		// size without encountering this limitation. Once the buffer is set, we can then resize the window back to the desired dimensions.
		// More details: https://learn.microsoft.com/en-us/windows/console/window-and-screen-buffer-size
		// Set console window size to minimal (1x1)

		if (fontWidth == 0 || fontHeight == 0)
		{
			fontWidth = _m_pixelWidth;
			fontHeight = _m_pixelHeight;

			if (fontWidth == 0 || fontHeight == 0)
			{
				throw std::runtime_error("Font width and height cannot be zero.");
			}
		}

		if (width == 0 || height == 0)
		{
			throw std::runtime_error("Width and height cannot be zero.");
		}

		WinCore::ConsoleConfig config{};
		config.cols = width;
		config.rows = height;
		config.fontW = fontWidth;
		config.fontH = fontHeight;
		config.borderless = true;
		config.disableResize = true;

		HANDLE h = _m_screenBuffer->GetConsoleHandle();

		WinCore::ConsoleApplied congfigApplied = WinCore::ApplyConsoleConfig(h, config);

		PixelBuffer* buffer = _m_screenBuffer.get();

		// 3) Update internal buffer + write region
		buffer->m_bufferSize.X = static_cast<SHORT>(congfigApplied.cols);
		buffer->m_bufferSize.Y = static_cast<SHORT>(congfigApplied.rows);

		_m_screenBuffer->m_writePosition = {
			0, 0,
			static_cast<SHORT>(congfigApplied.cols - 1),
			static_cast<SHORT>(congfigApplied.rows - 1)
		};

		_m_width = congfigApplied.cols > 0 ? congfigApplied.cols : _m_width;
		_m_height = congfigApplied.rows > 0 ? congfigApplied.rows : _m_height;

		if (congfigApplied.fontW == 0 || congfigApplied.fontH == 0)
		{
			throw std::runtime_error("Fatal: invalid font sizing");
		}

		_m_pixelWidth = congfigApplied.fontW;
		_m_pixelHeight = congfigApplied.fontH;
		
		ConsoleGraphX_Internal::LoggerManager::Instance().LogMessage("Screen", "Screen resized to " + std::to_string(_m_width) + "x" + std::to_string(_m_height));

		return width == congfigApplied.cols && height == congfigApplied.rows;
	}

	void Screen::SetScreenBuffer(ConsoleGraphX_Internal::PixelBuffer buffer)
	{
		SetPixelBuffer(std::make_unique<ConsoleGraphX_Internal::PixelBuffer>(buffer));
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

	//NOTE FOR THE FOLLOWING PALLET METHODS
	//	Inside the methos you'll see the following
	//		consoleInfo.srWindow.Bottom++;
	//		consoleInfo.srWindow.Right++;
	//	This is here because SetConsoleScreenBufferInfoEx shrinks the window by 1 for some reason
	//	to combat this we are incrementing the window size by 1

	void Screen::SetPalletColors_A(std::array<ConsoleGraphX::Color_CGX, 16>& paletteColors) 
	{
		CONSOLE_SCREEN_BUFFER_INFOEX consoleInfo{};
		consoleInfo.cbSize = sizeof(consoleInfo);

		GetConsoleScreenBufferInfoEx(_s_activeScreen->_m_screenBuffer->GetConsoleHandle(), &consoleInfo);

		for (size_t i = 0; i < paletteColors.size(); i++) 
		{
			consoleInfo.ColorTable[i] = RGB(paletteColors[i].r, paletteColors[i].g, paletteColors[i].b);
		}

		consoleInfo.srWindow.Bottom++;
		consoleInfo.srWindow.Right++;
		SetConsoleScreenBufferInfoEx(_s_activeScreen->_m_screenBuffer->GetConsoleHandle(), &consoleInfo);
	}

	// sets the pallet of the console in calling process
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

		consoleInfo.srWindow.Bottom++;
		consoleInfo.srWindow.Right++;
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

		consoleInfo.srWindow.Bottom++;
		consoleInfo.srWindow.Right++;
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
