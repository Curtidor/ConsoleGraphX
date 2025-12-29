#pragma once
#include <type_traits>
#include <functional>
#include "Engine\Core\Utils\meta_utils.h" // is_std_function

namespace ConsoleGraphX
{
    // Maps "event flavor" (std::function or raw ptr) -> actual stored callable type
    template <typename Func, typename... Args>
    using CallableTypeImpl =
        std::conditional_t<
        is_std_function<Func>::value,
        std::function<void(Args...)>,
        void(*)(Args...)
        >;
}
