#pragma once
#include <vector>
#include <atomic>
#include <algorithm>
#include <functional>
#include <memory>
#include <utility>
#include <type_traits>

#include "Engine\Core\Event\event_handle.h"
#include "Engine\Core\Utils\meta_utils.h" // is_valid_event_type, is_std_function
#include "Engine\Core\Event/event_callable.h"


namespace ConsoleGraphX
{
    /**
     * @brief Base class to manage event callbacks.
     *
     * @tparam Func The type tag for listener style (std::function<void(...)> or raw function pointer).
     * @tparam Args Variadic template arguments for the callback signature.
     */
    template <typename Func, typename... Args>
    class CGXEventBase
    {
        static_assert(is_valid_event_type<Func>::value,
            "Listener type must be a raw function pointer type or std::function<void(...)>.");

    public:
        using CallableType = CallableTypeImpl<Func, Args...>;
        using HandleType = EventHandle;

    protected:
        struct Entry
        {
            CallableType callback{};
            size_t id = 0;
        };

        std::vector<Entry> _m_callbacks;
        std::atomic<size_t> _m_nextHandle = 1;

        // Safe mutation during invoke (optional but strongly recommended)
        bool _m_isInvoking = false;
        std::vector<Entry> _m_pendingAdds;
        std::vector<size_t> _m_pendingRemoves;

    public:
        CGXEventBase() = default;
        CGXEventBase(const CGXEventBase&) = default;
        CGXEventBase& operator=(const CGXEventBase&) = default;
        CGXEventBase(CGXEventBase&&) noexcept = default;
        CGXEventBase& operator=(CGXEventBase&&) noexcept = default;

        /**
         * @brief Invoke all stored callbacks with forwarded arguments.
         */
        void Invoke(Args&&... args)
        {
            _m_isInvoking = true;

            for (auto& e : _m_callbacks)
                e.callback(std::forward<Args>(args)...);

            _m_isInvoking = false;
            _FlushPending();
        }

        /**
         * @brief Invoke with non-forwarded lvalue refs.
         */
        void InvokeNF(Args&... args)
        {
            _m_isInvoking = true;

            for (auto& e : _m_callbacks)
                e.callback(static_cast<Args&>(args)...);

            _m_isInvoking = false;
            _FlushPending();
        }

        /**
         * @brief Invoke with const lvalue refs.
         */
        void InvokeNFC(const Args&... args)
        {
            _m_isInvoking = true;

            for (auto& e : _m_callbacks)
                e.callback(static_cast<const Args&>(args)...);

            _m_isInvoking = false;
            _FlushPending();
        }

        /**
         * @brief Add a listener by moving the callback.
         */
        [[nodiscard]] HandleType AddListener(CallableType&& callback)
        {
            return _AddImpl(std::move(callback));
        }

        /**
         * @brief Add a captured lambda directly (e.g., `[this](...) {}`).
         * Same as AddListener; kept for readability.
         */
        [[nodiscard]] HandleType AddListenerLambda(CallableType&& lambda)
        {
            return _AddImpl(std::move(lambda));
        }

        /**
         * @brief Add a listener based on a weak_ptr.
         */
        template <typename T>
        [[nodiscard]] HandleType AddListener(std::weak_ptr<T> listener, void (T::* callbackFunction)(Args...))
        {
            CallableType weakCallback =
                [listener, callbackFunction](Args&&... a)
                {
                    if (auto sharedListener = listener.lock())
                        (sharedListener.get()->*callbackFunction)(std::forward<Args>(a)...);
                };

            return _AddImpl(std::move(weakCallback));
        }

        /**
         * @brief Add a listener based on a raw pointer (caller must ensure lifetime).
         * This is NOT safe unless you control lifetime; prefer weak_ptr when possible.
         */
        template <typename T>
        [[nodiscard]] HandleType AddListener(T* listener, void (T::* callbackFunction)(Args...))
        {
            CallableType callback =
                [listener, callbackFunction](Args&&... a)
                {
                    if (listener)
                        (listener->*callbackFunction)(std::forward<Args>(a)...);
                };

            return _AddImpl(std::move(callback));
        }

        /**
         * @brief Remove listener by handle (fast, reliable, works for all callable types).
         */
        void RemoveListener(HandleType handle)
        {
            if (!handle) return;

            if (_m_isInvoking)
            {
                _m_pendingRemoves.push_back(handle.id);
                return;
            }

            _RemoveById(handle.id);
        }

        /**
         * @brief Clear all listeners. Avoid calling from inside a listener if you can.
         */
        void ClearListeners()
        {
            if (_m_isInvoking)
            {
                _m_pendingRemoves.reserve(_m_pendingRemoves.size() + _m_callbacks.size());
                for (auto& e : _m_callbacks)
                    _m_pendingRemoves.push_back(e.id);
                return;
            }

            _m_callbacks.clear();
        }

    protected:
        /**
         * @brief Optional: Remove listeners by predicate (useful for raw function pointer events).
         * Derived classes can use this without needing access to internal removal logic.
         */
        template <typename Pred>
        void RemoveIf(Pred&& pred)
        {
            if (_m_isInvoking)
            {
                for (auto& e : _m_callbacks)
                {
                    if (pred(e))
                        _m_pendingRemoves.push_back(e.id);
                }
                return;
            }

            // Swap-remove in a loop to keep it O(n) without shifting
            for (size_t i = 0; i < _m_callbacks.size(); )
            {
                if (pred(_m_callbacks[i]))
                {
                    _m_callbacks[i] = std::move(_m_callbacks.back());
                    _m_callbacks.pop_back();
                    continue;
                }
                ++i;
            }
        }

    private:
        [[nodiscard]] HandleType _AddImpl(CallableType&& cb)
        {
            const size_t id = _m_nextHandle.fetch_add(1, std::memory_order_relaxed);
            Entry e{ std::move(cb), id };

            if (_m_isInvoking)
                _m_pendingAdds.push_back(std::move(e));
            else
                _m_callbacks.push_back(std::move(e));

            return HandleType{ id };
        }

        void _RemoveById(size_t id)
        {
            // swap-remove
            auto it = std::find_if(_m_callbacks.begin(), _m_callbacks.end(),
                [id](const Entry& e) { return e.id == id; });

            if (it == _m_callbacks.end())
                return;

            *it = std::move(_m_callbacks.back());
            _m_callbacks.pop_back();
        }

        void _FlushPending()
        {
            if (!_m_pendingRemoves.empty())
            {
                for (size_t id : _m_pendingRemoves)
                    _RemoveById(id);

                _m_pendingRemoves.clear();
            }

            if (!_m_pendingAdds.empty())
            {
                for (auto& e : _m_pendingAdds)
                    _m_callbacks.push_back(std::move(e));

                _m_pendingAdds.clear();
            }
        }
    };
}
