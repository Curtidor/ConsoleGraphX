#include <utility>
#include <cstdint>
#include "texture.h"


namespace ConsoleGraphX_Internal
{

    Texture::Texture()
        : _m_width(1), _m_height(1) 
    {
        _m_pixels = new CHAR_INFO[_m_width * _m_height];
    }

    Texture::Texture(uint32_t width, uint32_t height)
        : _m_width(width), _m_height(height)
    {
        _m_pixels = new CHAR_INFO[width * height];
    }

    Texture::Texture(uint32_t width, uint32_t height, int color)
        : _m_width(width), _m_height(height)
    {
        _m_pixels = new CHAR_INFO[width * height];

        for (size_t i = 0; i < static_cast<size_t>(width) * height; i++)
        {
            _m_pixels[i].Attributes = color;
            _m_pixels[i].Char.UnicodeChar = L'\x2588';
        }
    }

    Texture::~Texture()
    {
        delete[] _m_pixels;
    }

    Texture::Texture(Texture&& other) noexcept
        : _m_pixels(nullptr), _m_width(0), _m_height(0)
    {
        std::swap(_m_pixels, other._m_pixels);
        std::swap(_m_width, other._m_width);
        std::swap(_m_height, other._m_height);
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (this != &other) 
        {
            delete[] _m_pixels;
            _m_pixels = nullptr;
            _m_width = 0;
            _m_height = 0;

            std::swap(_m_pixels, other._m_pixels);
            std::swap(_m_width, other._m_width);
            std::swap(_m_height, other._m_height);
        }

        return *this;
    }

    uint32_t Texture::GetWidth() const
    {
        return _m_width;
    }

    uint32_t Texture::GetHeight() const
    {
        return _m_height;
    }

    CHAR_INFO* Texture::GetPixels()
    {
        return _m_pixels;
    }

};
