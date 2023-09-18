#pragma once
#include <Windows.h>
#include <algorithm>
#include "transform.h"

struct Sprite : Component
{
private:
	int m_width;
	int m_height;
	CHAR_INFO* m_pixels;

public:
	Transform m_transform;

	Sprite();
	Sprite(int width, int height);
	Sprite(int width, int height, int color);
	Sprite(int width, int height, CHAR_INFO* pixels);

	~Sprite() override;

	int GetWidth();
	int GetHeight();

	CHAR_INFO* GetPixels();
};