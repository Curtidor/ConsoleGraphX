#pragma once
#include <cstddef>

namespace ConsoleGraphX
{
    /// Lightweight listener token. Does NOT store the callback.
    struct EventHandle
    {
        size_t id = 0;

        constexpr explicit operator bool() const noexcept { return id != 0; }
        friend constexpr bool operator==(EventHandle a, EventHandle b) noexcept { return a.id == b.id; }
        friend constexpr bool operator!=(EventHandle a, EventHandle b) noexcept { return a.id != b.id; }
    };
}
