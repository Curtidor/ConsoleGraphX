#pragma once
#include <windows.h>
#include "component.h"
#include "vector2.h"

namespace ConsoleGraphX
{
    enum Color
    {
        Black = 0,
        DarkBlue = 1,
        DarkGreen = 2,
        DarkCyan = 3,
        DarkRed = 4,
        DarkMagenta = 5,
        DarkYellow = 6,
        Gray = 7,
        DarkGray = 8,
        Blue = 9,
        Green = 10,
        Cyan = 11,
        Red = 12,
        Magenta = 13,
        Yellow = 14,
        White = 15
    };



    struct Sprite : public ConsoleGraphX_Internal::Component
    {
    private:
        int _m_width;
        int _m_height;
        bool _m_isVisible;
        bool _m_isTransparent;
        CHAR_INFO* _m_pixels;

    public:
        int m_layer;
        Vector2 m_size;

    public:

        Sprite();
        Sprite(int width, int height, bool isTransparent = false);
        Sprite(int width, int height, int color, bool isTransparent = false);
        Sprite(int width, int height, CHAR_INFO* pixels, bool isTransparent = false);
        Sprite(const Sprite& other);
        ~Sprite() override;

        Sprite& operator=(const Sprite& other);

        void Clone(Sprite* sprite) const;
        void HideSprite();
        void ShowSprite();

        bool IsSpriteHidden() const;
        bool IsTransparent() const;

        int GetWidth() const;
        int GetHeight() const;

        const Vector2& Size() const;
        CHAR_INFO* GetPixels();
    };
};

