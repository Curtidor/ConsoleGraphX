#include "vector2.h"
#include <cmath>

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

double Vector2::Magnitude() const {
    return std::sqrt(x * x + y * y);
}

void Vector2::Normalize() {
    double magnitude = Magnitude();
    x /= magnitude;
    y /= magnitude;
}
