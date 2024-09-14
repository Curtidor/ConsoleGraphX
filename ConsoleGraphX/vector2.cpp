#include "vector2.h"
#include <cmath>

namespace ConsoleGraphX
{
    const Vector2 Vector2::left(-1.0f, 0.0f);
    const Vector2 Vector2::right(1.0f, 0.0f);
    const Vector2 Vector2::up(0.0f, 1.0f);
    const Vector2 Vector2::down(0.0f, -1.0f);

    Vector2::Vector2() : x(0), y(0) {}
    Vector2::Vector2(int x, int y) : x(static_cast<float>(x)), y(static_cast<float>(y)) {}
    Vector2::Vector2(float x, float y) : x(x), y(y) {}

    Vector2 Vector2::operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 Vector2::operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 Vector2::operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 Vector2::operator/(float scalar) const {
        return Vector2(x / scalar, y / scalar);
    }

    Vector2& Vector2::operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }


    Vector2& Vector2::operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2& Vector2::operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2& Vector2::operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    void Vector2::PerpendicularPoint(const Vector2& start, const Vector2& end, Vector2& oPerpendicularPoint)
    {
        oPerpendicularPoint.x = end.y - start.y;
        oPerpendicularPoint.y = end.x - start.x;
    }

    float Vector2::Magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    void Vector2::Normalize() {
        float magnitude = Magnitude();
        x /= magnitude;
        y /= magnitude;
    }
};


