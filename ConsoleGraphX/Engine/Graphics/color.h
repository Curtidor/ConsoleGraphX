#pragma once
#include <cstdint>

namespace ConsoleGraphX
{
	struct Color_CGX
	{
		uint8_t r, g, b;

		Color_CGX() : r(0), g(0), b(0) {}

		Color_CGX(uint8_t red, uint8_t green, uint8_t blue) : r(red), g(green), b(blue) {}

		Color_CGX(int red, int green, int blue)
			: r(static_cast<uint8_t>(red)),
			g(static_cast<uint8_t>(green)),
			b(static_cast<uint8_t>(blue)) {}

		Color_CGX(int grayscale)
			: r(static_cast<uint8_t>(grayscale)),
			g(static_cast<uint8_t>(grayscale)),
			b(static_cast<uint8_t>(grayscale)) {}
	};
}
