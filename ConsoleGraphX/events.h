#pragma once
#include <functional>
#include "event_base.h"
#include "event_handle.h"
#include <memory>

namespace ConsoleGraphX
{
    /**
     * @brief Class to store and invoke callbacks using std::function.
     *
     * @tparam Args Variadic template arguments for the callback.
     */
    template <typename... Args>
    class CGXEventArgs : public CGXEventBase<std::function<void(Args...)>, Args...>
    {};

  
    class CGXEvent : public CGXEventBase<std::function<void()>>
    {};

    /**
     * @brief Class to store and invoke callbacks using raw function pointers.
     *
     * @tparam Args Variadic template arguments for the callback.
     */
    template <typename... Args>
    class CGXRawEventArgs : public CGXEventBase<void(*)(Args...), Args...>
    {
    public:
        using CallableType = CallableTypeImpl<void(*)(Args...), Args...>;

        /**
        * @brief Remove listener by comparing function pointers.
        *
        * @param callback The callback function (raw function pointer) to remove.
        */
        void RemoveListener(const CallableType& callback)
        {
            this->_m_callbacks.erase(
                std::remove_if(this->_m_callbacks.begin(), this->_m_callbacks.end(),
                    [&callback](const EventCallBackHandle<CallableType, Args...>& handle)
                    {
                        return handle.m_callback == callback;
                    }),
                this->_m_callbacks.end());
        }
    };

    /**
     * @brief Class for handling captured lambdas.
     *
     * @tparam Args Variadic template arguments for the callback.
     */
    template <typename... Args>
    class CGXCapturedEventArgs : public CGXEventBase<std::function<void(Args...)>, Args...>
    {
    public:
        using CallableType = std::function<void(Args...)>;

        /**
        * @brief Add a listener by moving the callback to avoid copying.
        *
        * @param callback The callback function to add (moved).
        * @return EventCallBackHandle containing the unique handle and callback.
        */
        [[nodiscard]] EventCallBackHandle<CallableType, Args...> AddListener(CallableType&& callback) // This is here to enforce the return type should be stored
        {
            return this->CGXEventBase<CallableType, Args...>::AddListener(std::move(callback));
        }

        /**
        * @brief Remove listener by handle. This is the only way to remove listeners in captured events.
        *
        * @param handleToRemove The handle of the callback to remove.
        */
        void RemoveListener(const EventCallBackHandle<CallableType, Args...>& handleToRemove)
        {
            // Override the base class's remove by handle
            this->CGXEventBase<CallableType, Args...>::RemoveListener(handleToRemove);
        }

        /**
         * @brief Remove listener by comparing function pointers - disabled for captured events.
         */
        void RemoveListener(const CallableType& callback) = delete;  // Disables removing by function pointer
    };
};

