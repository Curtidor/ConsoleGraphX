#include "PCH_CGX.h"
#include "Engine/Math/vector2i.h"
#include "Engine/Math/vector2.h"

#include <cmath>
#include <limits>
#include <algorithm>

namespace ConsoleGraphX
{
    const Vector2i Vector2i::left(-1, 0);
    const Vector2i Vector2i::right(1, 0);
    const Vector2i Vector2i::up(0, 1);
    const Vector2i Vector2i::down(0, -1);

    static inline int32_t RoundToI32(float v) noexcept
    {
        // clamp for safety, then round
        const float lo = (float)(std::numeric_limits<int32_t>::min)();
        const float hi = (float)(std::numeric_limits<int32_t>::max)();
        v = std::clamp(v, lo, hi);
        return (int32_t)std::lrintf(v);
    }

    Vector2i::Vector2i(const Vector2& v) noexcept
        : x(RoundToI32(v.x)), y(RoundToI32(v.y))
    {
    }

    Vector2 Vector2i::ToVector2() const noexcept
    {
        return Vector2((float)x, (float)y);
    }
}
