#pragma once
#include <Windows.h>
#include <algorithm>
#include "transform.h"

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



struct Sprite : Component
{
private:
	int m_width;
	int m_height;
	CHAR_INFO* m_pixels;

public:

	Sprite();
	Sprite(int width, int height);
	Sprite(int width, int height, int color);
	Sprite(int width, int height, CHAR_INFO* pixels);

	~Sprite() override;

	int GetID() const override;

	int GetWidth();
	int GetHeight();

	CHAR_INFO* GetPixels();
};