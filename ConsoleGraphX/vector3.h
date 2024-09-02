#pragma once
#include "vector2.h"

namespace ConsoleGraphX
{

    class Vector3 {
    public:
        float x;
        float y;
        float z;

        static const Vector3 left;
        static const Vector3 right;
        static const Vector3 up;
        static const Vector3 down;
        static const Vector3 forward;
        static const Vector3 backward;
        static const Vector3 zero;

        Vector3(float x = 0, float y = 0, float z = 0);

        Vector3 operator+(const Vector3& other) const;
        Vector3& operator+=(const Vector2& other);
        Vector3& operator+=(const Vector3& other);

        Vector3 operator-(const Vector3& other) const;
        Vector3& operator-=(const Vector2& other);
        Vector3& operator-=(const Vector3& other);

        Vector3 operator*(float scalar) const;
        Vector3& operator*=(float scalar);

        Vector3 operator/(float scalar) const;
        Vector3& operator/=(float scalar);


        float Magnitude() const;
        void AbsVector();
        void Normalize();
    };

};
