#pragma once
#include <chrono>
#include <string>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include "window.h"

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)
#define INCREMENT_COUNTER(name, value) CGXProfiler::Instance().IncrementCounter(name, value)


namespace ConsoleGraphX_Internal
{
    class CGXProfiler
    {
    public:
        static void Initialize()
        {
            assert(!_s_instance);

            _s_instance = new CGXProfiler();
        }

        static CGXProfiler& Instance()
        {
            assert(_s_instance);
            return *_s_instance;
        }

        static void ShutDown()
        {
            assert(_s_instance);

            delete _s_instance;
        }

        void AttachWindow(ConsoleGraphX::CrossProcessWindow* window)
        {
            _m_window = window;
        }

        // Adds time to a named timer
        void AddTime(const std::string& name, double duration) 
        {
            _m_timers[name].totalTime += duration;
            _m_timers[name].count++;
        }

        // Increments a named counter
        void IncrementCounter(const std::string& name, int value = 1) 
        {
            _m_counters[name] += value;
        }

        void DisplayMetrics()
        {
            if (!_m_window)
            {
                return;
            }

            int y = 0;

            // Write the header
            _m_window->WriteText("==================== CGX Profiler Metrics ====================", 2, y++);

            // Temporary buffer for the current line
            std::string buffer;
            std::string spaces(30, ' ');

            // display timers
            for (auto& [name, timer] : _m_timers)
            {
                buffer = name + ": " + std::to_string(timer.totalTime) + "ms (" + std::to_string(timer.count) + " calls)";
                _m_window->WriteText(spaces, 2, y);
                _m_window->WriteText(buffer, 2, y++);

                // Reset timer data after displaying
                timer.totalTime = 0.0f;
                timer.count = 0;
            }

            // display counters
            for (auto& [name, count] : _m_counters)
            {
                buffer = name + ": " + std::to_string(count);

                _m_window->WriteText(spaces, 2, y);  
                _m_window->WriteText(buffer, 2, y++);


                // Reset counter data after displaying
                count = 0;
            }
        }

    private:
        // Private constructor for singleton
        CGXProfiler() = default;

        static inline CGXProfiler* _s_instance = nullptr;
        ConsoleGraphX::CrossProcessWindow* _m_window = nullptr;
        // Struct to store timer information
        struct TimerData
        {
            double totalTime = 0.0;
            int count = 0;
        };

        // Timer map and counter map
        std::unordered_map<std::string, TimerData> _m_timers;
        std::unordered_map<std::string, int> _m_counters;
    };

    // Scoped timer helper class
    class Timer
    {
    public:
        Timer(const std::string& name)
            : _name(name), _start(std::chrono::high_resolution_clock::now()) {}

        ~Timer()
        {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration = end - _start;
            CGXProfiler::Instance().AddTime(_name, duration.count());
        }

    private:
        std::string _name;
        std::chrono::time_point<std::chrono::high_resolution_clock> _start;
    };

}
