#pragma once
#include <type_traits>
#include <functional>
#include "meta_utils.h"

namespace ConsoleGraphX
{
    // Helper to determine the callable type (std::function or raw pointer)
    template <typename Func, typename... Args>
    using CallableTypeImpl = typename std::conditional<
        is_std_function<Func>::value,
        std::function<void(Args...)>,       // If Func is std::function, use this
        void(*)(Args...)                   // If Func is raw pointer, use this
    >::type;

    /**
     * @brief Struct to store the callback and its unique handle.
     *
     * @tparam Func Callable type (either raw function pointer or std::function).
     * @tparam Args Variadic template arguments for the callback.
     */
    template <typename Func, typename... Args>
    struct EventCallBackHandle
    {
        CallableTypeImpl<Func, Args...> m_callback;  ///< The callback function.
        size_t handle;  ///< Unique handle for the callback.

        /**
         * @brief Constructor to initialize the callback and handle.
         *
         * @param callback The callback function (moved).
         * @param h A unique handle.
         */
        EventCallBackHandle(CallableTypeImpl<Func, Args...>&& callback, size_t h)
            : m_callback(std::move(callback)), handle(h) {}
    };

};
