#pragma once

namespace ConsoleGraphX
{
    class Vector2 
    {
    public:
        float x;
        float y;

        static const Vector2 left;
        static const Vector2 right;
        static const Vector2 up;
        static const Vector2 down;

        Vector2();
        Vector2(int x, int y);
        Vector2(float x, float y);

        Vector2 operator+(const Vector2& other) const;
        Vector2& operator+=(const Vector2& other);

        Vector2 operator-(const Vector2& other) const;
        Vector2& operator-=(const Vector2& other);

        Vector2 operator*(float scalar) const;
        Vector2& operator*=(float scalar);

        Vector2 operator/(float scalar) const;
        Vector2& operator/=(float scalar);

        /**
        * @brief Calculates a point on the line perpendicular to a given line segment.
        *
        * This function calculates a point on the line that is perpendicular to the line segment
        * defined by the provided 'start' and 'end' points.
        *
        * @param start The starting point of the line segment.
        * @param end The ending point of the line segment.
        * @param o_perpendicular_point Output parameter for the calculated perpendicular point.
        *
        * @note The 'o_perpendicular_point' parameter will be modified to contain the calculated point.
        */
        static void PerpendicularPoint(const Vector2& start, const Vector2& end, Vector2& o_normal_point);

        float Magnitude() const;
        void Normalize();
    };
};

