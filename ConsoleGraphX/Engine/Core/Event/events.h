#pragma once
#include <functional>
#include "Engine\Core\Event\event_base.h"

namespace ConsoleGraphX
{
    /**
     * @brief Standard event that uses std::function<void(Args...)>.
     */
    template <typename... Args>
    class CGXEventArgs : public CGXEventBase<std::function<void(Args...)>, Args...>
    {
    };

    /**
     * @brief Standard event with no args.
     */
    class CGXEvent : public CGXEventBase<std::function<void()>>
    {
    };

    /**
     * @brief Raw function pointer event (fastest, no captures).
     * Supports RemoveListener(callback) because equality is meaningful for function pointers.
     */
    template <typename... Args>
    class CGXRawEventArgs : public CGXEventBase<void(*)(Args...), Args...>
    {
    public:
        using Base = CGXEventBase<void(*)(Args...), Args...>;
        using CallableType = typename Base::CallableType;

        void RemoveListener(CallableType callback)
        {
            this->RemoveIf([&](const typename Base::Entry& e)
                {
                    return e.callback == callback;
                });
        }
    };

    /**
     * @brief Captured-lambda-friendly event.
     * Same as CGXEventArgs, but you can conceptually use it where you want to enforce "store the handle".
     *
     * If you want strict enforcement, keep [[nodiscard]] on AddListenerLambda and don’t offer a void-return overload.
     */
    template <typename... Args>
    class CGXCapturedEventArgs : public CGXEventBase<std::function<void(Args...)>, Args...>
    {
    };
}
