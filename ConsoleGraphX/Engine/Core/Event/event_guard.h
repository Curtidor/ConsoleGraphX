#pragma once
#include <optional>
#include <utility>

namespace ConsoleGraphX
{
    /**
     * @brief RAII guard that auto-unsubscribes a listener when destroyed.
     *
     * Usage:
     *   CGXEventGuard<decltype(myEvent)> guard(myEvent, myEvent.AddListenerLambda(...));
     */
    template <typename EventT>
    class CGXEventGuard
    {
        EventT* _event = nullptr;
        std::optional<typename EventT::HandleType> _handle;

    public:
        CGXEventGuard() = default;

        CGXEventGuard(EventT& event, typename EventT::HandleType handle)
            : _event(&event), _handle(handle) {
        }

        CGXEventGuard(const CGXEventGuard&) = delete;
        CGXEventGuard& operator=(const CGXEventGuard&) = delete;

        CGXEventGuard(CGXEventGuard&& other) noexcept
            : _event(std::exchange(other._event, nullptr)),
            _handle(std::move(other._handle)) {
        }

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

        [[nodiscard]] bool HasHandle() const noexcept { return _handle.has_value(); }
    };
}
