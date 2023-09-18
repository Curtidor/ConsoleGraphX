#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <queue>

/**
 * @brief A generic event dispatcher that supports event prioritization.
 * @tparam T The event data type.
 */
template <typename T>
class Dispatcher {
private:
    struct Listener {
        std::function<void(const T&)> callback;
        int priority;

        bool operator<(const Listener& other) const {
            return priority < other.priority;
        }
    };

    // Stores registered listeners for each event.
    static std::unordered_map<std::string, std::priority_queue<Listener>> _s_event_registry;

public:
    /**
     * @brief Notifies all listeners for a specific event with event data.
     * @param event_name The name of the event.
     * @param eventData The event data.
     */
    static void Notify(const std::string& event_name, const T& eventData) {
        auto it = _s_event_registry.find(event_name);

        if (it != _s_event_registry.end()) {
            while (!it->second.empty()) {
                const auto& listener = it->second.top();
                listener.callback(eventData);
                it->second.pop();
            }
        }
    }

    /**
     * @brief Registers a listener for a specific event with an optional priority.
     * @param event_name The name of the event.
     * @param callback The listener callback.
     * @param priority The priority level (default is 0).
     */
    static void RegisterListener(const std::string& event_name, std::function<void(const T&)> callback, int priority = 0) {
        _s_event_registry[event_name].push({ callback, priority });
    }

    /**
     * @brief Deregisters a specific listener for a given event with an optional priority.
     * @param event_name The name of the event.
     * @param callback The listener callback to deregister.
     * @param priority The priority level (default is 0).
     */
    static void DeregisterListener(const std::string& event_name, std::function<void(const T&)> callback, int priority = 0) {
        auto it_event = _s_event_registry.find(event_name);

        // No priority queue found for the given event name
        if (it_event == _s_event_registry.end())
            return;

        auto& listener_queue = it_event->second;
        std::priority_queue<Listener> new_queue; // Create a new queue without the specified listener

        while (!listener_queue.empty()) {
            const auto& listener = listener_queue.top();
            if (!(listener.callback == callback && listener.priority == priority)) {
                new_queue.push(listener); // Keep listeners that are not the one to be removed
            }
            listener_queue.pop();
        }

        // Replace the old priority queue with the new one (without the specified listener)
        std::swap(listener_queue, new_queue);
    }
};

// Static member initialization
template <typename T>
std::unordered_map<std::string, std::priority_queue<typename Dispatcher<T>::Listener>> Dispatcher<T>::_s_event_registry;