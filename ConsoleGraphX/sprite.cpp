#include "sprite.h"

Sprite::Sprite() 
	:m_width(1), m_height(1), m_pixels(new CHAR_INFO[1])
{}

Sprite::Sprite(int width, int height)
{
	CHAR_INFO c = { L'█', FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY };
	std::fill(m_pixels, m_pixels + (m_width * m_height), c);
}

Sprite::Sprite(int width, int height, int color)
	: m_width(width), m_height(height), m_pixels(new CHAR_INFO[width * height])
{
	CHAR_INFO c = { L'█', color };
	std::fill(m_pixels, m_pixels + (m_width * m_height), c);
}

Sprite::Sprite(int width, int height, CHAR_INFO* pixels)
	:m_width(width), m_height(height), m_pixels(pixels)
{}

Sprite::~Sprite()
{
	delete[] this->m_pixels;
}

int Sprite::GetHeight() { return this->m_height; }
int Sprite::GetWidth() { return this->m_width; }

CHAR_INFO* Sprite::GetPixels() { return this->m_pixels; }