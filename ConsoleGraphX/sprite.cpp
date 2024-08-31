#include <windows.h>
#include <algorithm>
#include "sprite.h"
#include "vector2.h"

namespace ConsoleGraphX
{
	Sprite::Sprite() : _m_width(1), _m_height(1), _m_pixels(new CHAR_INFO[1]), _m_isVisible(true), _m_isTransparent(false), m_layer(0), m_size(0,0)
	{}

	Sprite::Sprite(int width, int height, bool isTransparent) : _m_width(width), _m_height(height), _m_pixels(new CHAR_INFO[width * height]), _m_isVisible(true), _m_isTransparent(isTransparent), m_layer(0), m_size(width, height)
	{
		CHAR_INFO c = { L'█', Color::Magenta };
		std::fill(_m_pixels, _m_pixels + (width * height), c);
	}

	Sprite::Sprite(int width, int height, int color, bool isTransparent) : _m_width(width), _m_height(height), _m_pixels(new CHAR_INFO[width * height]), _m_isVisible(true), _m_isTransparent(isTransparent), m_layer(0), m_size(width, height)
	{
		CHAR_INFO c = { L'█', color };
		std::fill(_m_pixels, _m_pixels + (width * height), c);
	}

	Sprite::Sprite(int width, int height, CHAR_INFO* pixels, bool isTransparent) : _m_width(width), _m_height(height), _m_pixels(pixels), _m_isVisible(true), _m_isTransparent(isTransparent), m_layer(0), m_size(width, height)
	{
	}

	Sprite::Sprite(const Sprite& other)
		: _m_width(other._m_width), _m_height(other._m_height), _m_isVisible(other._m_isVisible), _m_isTransparent(other._m_isTransparent), m_layer(other.m_layer), m_size(other.m_size)
	{
		_m_pixels = new CHAR_INFO[_m_width * _m_height];

		std::copy(other._m_pixels, other._m_pixels + (_m_width * _m_height), _m_pixels);
	}

	Sprite::~Sprite()
	{
		delete[] _m_pixels;
	}

	void Sprite::Clone(Sprite* sprite) const
	{
		*sprite = Sprite(*this);
	}

	Sprite& Sprite::operator=(const Sprite& other) 
	{
		if (this == &other) 
		{
			return *this;
		}

		delete[] _m_pixels;

		_m_width = other._m_width;
		_m_height = other._m_height;
		_m_isVisible = other._m_isVisible;
		_m_isTransparent = other._m_isTransparent;
		m_layer = other.m_layer;
		m_size = other.m_size;

		_m_pixels = new CHAR_INFO[_m_width * _m_height];
		std::copy(other._m_pixels, other._m_pixels + (_m_width * _m_height), _m_pixels);

		return *this;
	}

	const Vector2& Sprite::Size() const
	{
		return m_size;
	}

	void Sprite::HideSprite() { _m_isVisible = false; }
	void Sprite::ShowSprite() { _m_isVisible = true; }

	bool Sprite::IsSpriteHidden() const { return _m_isVisible; }
	bool Sprite::IsTransparent() const { return _m_isTransparent; }

	int Sprite::GetHeight() const { return _m_height; }
	int Sprite::GetWidth() const { return _m_width; }


	CHAR_INFO* Sprite::GetPixels() { return _m_pixels; }
};