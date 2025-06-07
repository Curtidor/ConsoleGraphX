#pragma once
#include <unordered_map>
#include <string>
#include <functional>
#include <queue>

namespace ConsoleGraphX_Internal
{
    template <typename T>
    class Dispatcher 
    {
    private:
        struct Listener
        {
        public:

            std::function<void(const T&)> m_callback;
            std::string m_eventName;
            int m_priority;

            Listener(std::function<void(const T&)> callback, const std::string& eventName, int priority = 0)
                : m_callback(callback), m_eventName(eventName), m_priority(priority) 
            {}

            bool operator<(const Listener& other) const 
            {
                return m_priority < other.m_priority;
            }

            bool operator==(const Listener& other) const
            {
                return m_callback == other.m_callback && m_priority == other.m_priority && m_eventName == other.m_eventName;
            }
        };

        static std::unordered_map<std::string, std::priority_queue<Listener>> _s_eventRegistry;

    public:
        /**
       * @brief Notifies all listeners for a specific event with event data.
       * @param event_name The name of the event.
       * @param eventData The event data.
       */
        static void Notify(const std::string& eventName, const T& eventData) 
        {
            const auto& it = _s_eventRegistry.find(eventName);
            if (it == _s_eventRegistry.end())
                return;

            std::priority_queue<Listener> queueCopy = it->second;
            while (!queueCopy.empty()) {

                Listener l = queueCopy.top();
                l.m_callback(eventData);
                queueCopy.pop();
            }
        }
        
        /**
        * @brief Registers a listener for a specific event with an optional priority.
        * @param event_name The name of the event.
        * @param callback The listener callback.
        * @param priority The priority level (default is 0).
        */
        static void RegisterListener(const std::string& eventName, std::function<void(const T&)> callback, int priority = 0) 
        {
            _s_eventRegistry[eventName].emplace(callback, eventName, priority);
        }
       
        /**
        * @brief Deregisters a specific listener for a given event with an optional priority.
        * @param event_name The name of the event.
        * @param callback The listener callback to deregister.
        * @param priority The priority level (default is 0).
        */
        static void DeregisterListener(const std::string& eventName, std::function<void(const T&)> callback, int priority = 0) 
        {
            auto it = _s_eventRegistry.find(eventName);

            if (it == _s_eventRegistry.end())
                return;

            std::priority_queue<Listener>& listenerQueue = it->second;
            std::priority_queue<Listener> newQueue;

            while (!listenerQueue.empty()) {
                const Listener& listener = listenerQueue.top();
                if (!(listener.callback == callback && listener.m_eventName == eventName && listener.priority == priority)) 
                {
                    newQueue.push(listener);
                }
                listenerQueue.pop();
            }

            // Swap the content of the existing queue with the new one
            std::swap(it->second, newQueue);
        }
    };

    template<typename T>
    std::unordered_map<std::string, std::priority_queue<typename Dispatcher<T>::Listener>> Dispatcher<T>::_s_eventRegistry;
}
