#pragma once
#include <windows.h> 
#include <cstdint>


namespace ConsoleGraphX_Internal
{

    struct Texture
    {
    private:
        CHAR_INFO* _m_pixels; 
        uint32_t _m_width;         
        uint32_t _m_height;      

    public:
        Texture();
        Texture(uint32_t w, uint32_t h);
        Texture(uint32_t width, uint32_t height, int color);
        ~Texture();

        // don't allow textures to be copied
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;

        CHAR_INFO* GetPixels();
    };

};
