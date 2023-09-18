#pragma once
#include <cmath>

class Vector2 {
public:
    float x;
    float y;

    static const Vector2 left;
    static const Vector2 right;
    static const Vector2 up;
    static const Vector2 down;

    Vector2(float x = 0, float y = 0);

    Vector2 operator+(const Vector2& other) const;
    Vector2& operator+=(const Vector2& other);

    Vector2 operator-(const Vector2& other) const;
    Vector2& operator-=(const Vector2& other);

    Vector2 operator*(float scalar) const;
    Vector2& operator*=(float scalar);

    Vector2 operator/(float scalar) const;
    Vector2& operator/=(float scalar);

    double Magnitude() const;
    void Normalize();
};
#pragma once
