#include "vector2.h"
#include <cmath>

namespace ConsoleGraphX
{
    const Vector2 Vector2::left(-1.0f, 0.0f);
    const Vector2 Vector2::right(1.0f, 0.0f);
    const Vector2 Vector2::up(0.0f, 1.0f);
    const Vector2 Vector2::down(0.0f, -1.0f);

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

    void Vector2::PerpendicularPoint(const Vector2& start, const Vector2& end, Vector2& o_perpendicular_point)
    {
        o_perpendicular_point.x = end.y - start.y;
        o_perpendicular_point.y = end.x - start.x;
    }

    double Vector2::Magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    void Vector2::Normalize() {
        double magnitude = Magnitude();
        x /= magnitude;
        y /= magnitude;
    }
};


