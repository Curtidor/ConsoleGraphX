#pragma once
#include <optional>
#include "./Engine/Core/Event/event_base.h"

namespace ConsoleGraphX
{
    template <typename CallableType, typename... Args>
    class CGXEventGuard
    {
        CGXEventBase<CallableType, Args...>* _event = nullptr;
        std::optional<EventCallBackHandle<CallableType, Args...>> _handle;

    public:
        CGXEventGuard() = default;

        CGXEventGuard(CGXEventBase<CallableType, Args...>& event, EventCallBackHandle<CallableType, Args...> handle)
            : _event(&event), _handle(std::move(handle)) {}

        // No copy
        CGXEventGuard(const CGXEventGuard&) = delete;
        CGXEventGuard& operator=(const CGXEventGuard&) = delete;

        // Move support
        CGXEventGuard(CGXEventGuard&& other) noexcept
            : _event(std::exchange(other._event, nullptr)),
            _handle(std::move(other._handle)) {}

        CGXEventGuard& operator=(CGXEventGuard&& other) noexcept
        {
            if (this != &other)
            {
                Reset();
                _event = std::exchange(other._event, nullptr);
                _handle = std::move(other._handle);
            }
            return *this;
        }

        ~CGXEventGuard()
        {
            Reset();
        }

        void Reset()
        {
            if (_event && _handle)
                _event->RemoveListener(*_handle);
            _event = nullptr;
            _handle.reset();
        }
    };
}
