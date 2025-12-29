#pragma once
#include <cstdint>

namespace ConsoleGraphX
{
    class Vector2; // forward declare your float vector

    class Vector2i
    {
    public:
        int32_t x;
        int32_t y;

        static const Vector2i left;
        static const Vector2i right;
        static const Vector2i up;
        static const Vector2i down;

        constexpr Vector2i() noexcept : x(0), y(0) {}
        constexpr Vector2i(int32_t x_, int32_t y_) noexcept : x(x_), y(y_) {}

        // Explicit on purpose: don't allow silent float->int narrowing.
        explicit Vector2i(const Vector2& v) noexcept;

        // Basic ops (all cheap)
        constexpr Vector2i operator+(const Vector2i& o) const noexcept { return { x + o.x, y + o.y }; }
        constexpr Vector2i operator-(const Vector2i& o) const noexcept { return { x - o.x, y - o.y }; }

        constexpr Vector2i& operator+=(const Vector2i& o) noexcept { x += o.x; y += o.y; return *this; }
        constexpr Vector2i& operator-=(const Vector2i& o) noexcept { x -= o.x; y -= o.y; return *this; }

        constexpr Vector2i operator*(int32_t s) const noexcept { return { x * s, y * s }; }
        constexpr Vector2i& operator*=(int32_t s) noexcept { x *= s; y *= s; return *this; }

        // Integer division: truncates toward zero. Caller must ensure s != 0.
        constexpr Vector2i operator/(int32_t s) const noexcept { return { x / s, y / s }; }
        constexpr Vector2i& operator/=(int32_t s) noexcept { x /= s; y /= s; return *this; }

        constexpr bool operator==(const Vector2i& o) const noexcept { return x == o.x && y == o.y; }
        constexpr bool operator!=(const Vector2i& o) const noexcept { return !(*this == o); }

        Vector2 ToVector2() const noexcept; // convert back to float vector
    };
}
