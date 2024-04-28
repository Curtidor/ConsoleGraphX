#include "sprite.h"
#include <windows.h>
#include "component.h"
#include "vector2.h"

namespace ConsoleGraphX
{
	Sprite::Sprite() : _m_width(1), _m_height(1), _m_pixels(new CHAR_INFO[1]), _m_isVisible(true), _m_isTransparent(false), m_layer(0)
	{}

	Sprite::Sprite(int width, int height, bool isTransparent) : _m_width(width), _m_height(height), _m_pixels(new CHAR_INFO[width * height]), _m_isVisible(true), _m_isTransparent(isTransparent), m_layer(0)
	{
		CHAR_INFO c = { L'█', Color::Magenta };
		std::fill(_m_pixels, _m_pixels + (width * height), c);
	}

	Sprite::Sprite(int width, int height, int color, bool isTransparent) : _m_width(width), _m_height(height), _m_pixels(new CHAR_INFO[width * height]), _m_isVisible(true), _m_isTransparent(isTransparent), m_layer(0)
	{
		CHAR_INFO c = { L'█', color };
		std::fill(_m_pixels, _m_pixels + (width * height), c);
	}

	Sprite::Sprite(int width, int height, CHAR_INFO* pixels, bool isTransparent) : _m_width(width), _m_height(height), _m_pixels(pixels), _m_isVisible(true), _m_isTransparent(isTransparent), m_layer(0)
	{
	}

	Sprite::Sprite(const Sprite& other)
		: _m_width(other._m_width), _m_height(other._m_height), _m_isVisible(other._m_isVisible), _m_isTransparent(other._m_isTransparent), m_layer(other.m_layer)
	{
		_m_pixels = new CHAR_INFO[_m_width * _m_height];

		std::copy(other._m_pixels, other._m_pixels + (_m_width * _m_height), _m_pixels);
	}

	Sprite::~Sprite()
	{
		delete[] this->_m_pixels;
	}

	Vector2 Sprite::Size()
	{
		return{ static_cast<float>(_m_width), static_cast<float>(_m_height) };
	}

	int Sprite::GetID() const { return ComponentID::sprite; }

	ConsoleGraphX_Interal::Component* Sprite::Clone() const
	{
		Sprite* clone = new Sprite(*this);

		return clone;
	}

	void Sprite::HideSprite() { this->_m_isVisible = false; }
	void Sprite::ShowSprite() { this->_m_isVisible = true; }

	bool Sprite::IsSpriteHidden() { return this->_m_isVisible; }
	bool Sprite::IsTransparent() { return this->_m_isTransparent; }

	int Sprite::GetHeight() { return this->_m_height; }
	int Sprite::GetWidth() { return this->_m_width; }


	CHAR_INFO* Sprite::GetPixels() { return this->_m_pixels; }
};