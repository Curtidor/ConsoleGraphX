#include "vector3.h"
#include <cmath>
#include <cstdlib>
#include "vector2.h"

const Vector3 Vector3::left(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::right(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::up(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::down(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::forward(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::backward(0.0f, 0.0f, -1.0f);
const Vector3 Vector3::zero(0.0f, 0.0f, 0.0f);

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

Vector3 Vector3::operator+(const Vector3& other) const {
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3 Vector3::operator-(const Vector3& other) const {
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::operator*(float scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3 Vector3::operator/(float scalar) const {
    return Vector3(x / scalar, y / scalar, z / scalar);
}

double Vector3::Magnitude() const {
    return std::sqrt(x * x + y * y + z * z);
}

void Vector3::Normalize() {
    double magnitude = Magnitude();
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;
}

void Vector3::AbsVector()
{
    x = std::abs(x);
    y = std::abs(y);
    z = std::abs(z);
}

Vector3& Vector3::operator+=(const Vector3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vector3& Vector3::operator+=(const Vector2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector3& Vector3::operator-=(const Vector2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector3& Vector3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3& Vector3::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}
