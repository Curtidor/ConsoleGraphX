#pragma once
#include <vector>
#include <memory>
#include <atomic>
#include "event_handle.h"
#include "meta_utils.h"

namespace ConsoleGraphX
{
    enum EventType
    {
        Base = 0,
        KeyPressed, KeyReleased, MousePressed, MouseReleased, MouseScrolled,
        OnRenderStart, OnRenderFinish
    };

    /**
     * @brief Base class to manage event callbacks.
     *
     * @tparam Func The type of the listener function.
     * @tparam Args Variadic template arguments for the callback.
     */
    template <typename Func, typename... Args>
    class CGXEventBase
    {
        static_assert(is_valid_event_type<Func>::value, "Listener type must be a raw pointer or std::function");

    protected:
        std::vector<EventCallBackHandle<Func, Args...>> _m_callbacks;
        std::atomic<size_t> _m_nextHandle = 1;  ///< Atomic counter for generating unique handles.

    public:
        using CallableType = CallableTypeImpl<Func, Args...>;

        /**
        * @brief Invoke all stored callbacks with forwarded arguments.
        *
        * @param args Arguments to be passed to the callbacks.
        */
        void Invoke(Args&&... args)
        {
            for (auto& handle : this->_m_callbacks)
            {
                handle.m_callback(std::forward<Args>(args)...);
            }
        }

        void InvokeNF(Args&... args)
        {
            for (auto& handle : this->_m_callbacks)
            {
                handle.m_callback(static_cast<Args&>(args)...);
            }
        }

        void InvokeNFC(const Args&... args)
        {
            for (auto& handle : this->_m_callbacks)
            {
                handle.m_callback(static_cast<const Args&>(args)...);
            }
        }

        /**
        * @brief Add a listener by moving the callback to avoid copying.
        *
        * @param callback The callback function to add (moved).
        * @return EventCallBackHandle containing the unique handle and callback.
        */
        EventCallBackHandle<CallableType, Args...> AddListener(CallableType&& callback)
        {
            size_t handle = _m_nextHandle.fetch_add(1, std::memory_order_relaxed);
            _m_callbacks.emplace_back(std::move(callback), handle);
            return _m_callbacks.back();
        }

        /**
        * @brief Add a listener based on a weak pointer.
        *
        * @tparam T The type of the object managed by the weak pointer.
        * @param listener The weak pointer to the object.
        * @param callbackFunction The member function to call when the event is invoked.
        * @return EventCallBackHandle containing the unique handle and callback.
        */
        template <typename T>
        [[nodiscard]] EventCallBackHandle<CallableType, Args...> AddListener(std::weak_ptr<T> listener, void(T::* callbackFunction)(Args...))
        {
            size_t handle = _m_nextHandle.fetch_add(1, std::memory_order_relaxed);

            // Create a lambda that only invokes the callback if the weak_ptr can be locked
            CallableType weakCallback = [listener, callbackFunction](Args&&... args) {
                if (auto sharedListener = listener.lock()) {
                    (sharedListener.get()->*callbackFunction)(std::forward<Args>(args)...);
                }
                };

            _m_callbacks.emplace_back(std::move(weakCallback), handle);
            return _m_callbacks.back();
        }

        /**
         * @brief Remove listener by comparing function pointers.
         *
         * @param callback The callback function to remove.
         */
        void RemoveListener(const CallableType& callback)
        {
            _m_callbacks.erase(
                std::remove_if(_m_callbacks.begin(), _m_callbacks.end(),
                    [&callback](const EventCallBackHandle<CallableType, Args...>& handle)
                    {
                        return handle.m_callback.target<void(*)(Args...)>() == callback.target<void(*)(Args...)>();
                    }),
                _m_callbacks.end());
        }

        /**
         * @brief Remove listener by handle.
         *
         * @param handleToRemove The handle of the callback to remove.
         */
        void RemoveListener(const EventCallBackHandle<CallableType, Args...>& handleToRemove)
        {
            _m_callbacks.erase(
                std::remove_if(_m_callbacks.begin(), _m_callbacks.end(),
                    [&handleToRemove](const EventCallBackHandle<CallableType, Args...>& handle)
                    {
                        return handle.handle == handleToRemove.handle;
                    }),
                _m_callbacks.end());
        }
    };

};

