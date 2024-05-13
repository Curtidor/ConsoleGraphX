#pragma once
#include <unordered_map>
#include <functional>
#include <queue>
#include "debugger.h"

namespace ConsoleGraphX_Interal
{
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
        static std::unordered_map<std::string, std::priority_queue<Listener>> _s_eventRegistry;

    public:
        /**
         * @brief Notifies all listeners for a specific event with event data.
         * @param event_name The name of the event.
         * @param eventData The event data.
         */
        static void Notify(const std::string& event_name, const T& eventData) {
            auto it = _s_eventRegistry.find(event_name);

            if (it != _s_eventRegistry.end()) {
                std::priority_queue<Listener> listenerQueueCopy = it->second;

                // TODO:
                // listeners should be put into a queue then called if they are called within the notify method like they are now
                // it could lead to a stack over flow if two events call each other
                while (!listenerQueueCopy.empty()) {
                    Listener listener = listenerQueueCopy.top();
                    listener.callback(eventData);
                    listenerQueueCopy.pop();
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
            _s_eventRegistry[event_name].push({ callback, priority });
        }

        /**
         * @brief Deregisters a specific listener for a given event with an optional priority.
         * @param event_name The name of the event.
         * @param callback The listener callback to deregister.
         * @param priority The priority level (default is 0).
         */
        static void DeregisterListener(const std::string& event_name, std::function<void(const T&)> callback, int priority = 0) {
            auto eventNameNQueue = _s_eventRegistry.find(event_name);

            // No priority queue found for the given event name
            if (eventNameNQueue == _s_eventRegistry.end())
                return;

            auto& listenerQueue = eventNameNQueue->second;
            std::priority_queue<Listener> newQueue; // Create a new queue without the specified listener

            while (!listenerQueue.empty()) {
                const auto& listener = listenerQueue.top();
                if (!(listener.callback == callback && listener.priority == priority)) {
                    newQueue.push(listener); // Keep listeners that are not the one to be removed
                }
                listenerQueue.pop();
            }

            // Replace the old priority queue with the new one (without the specified listener)
            std::swap(listenerQueue, newQueue);
        }
    };

    // Static member initialization
    template <typename T>
    std::unordered_map<std::string, std::priority_queue<typename Dispatcher<T>::Listener>> Dispatcher<T>::_s_eventRegistry;
};


