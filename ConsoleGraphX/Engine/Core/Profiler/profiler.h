#pragma once
#include <chrono>
#include <string>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include "Engine\Core\Window\window.h"

#ifdef Editor
    #define PROFILE_SCOPE(name) Timer timer##__LINE__(name)
    #define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)
    #define INCREMENT_COUNTER(name, value) CGXProfiler::Instance().IncrementCounter(name, value)
#else
    #define PROFILE_SCOPE(name)
    #define PROFILE_FUNCTION()
    #define INCREMENT_COUNTER(name, value)
#endif

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

        // adds time to a named timer
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

            _m_window->WriteText("==================== CGX Profiler Metrics ====================", 2, y++);

            // temporary buffer for the current line
            std::string buffer;
            std::string spaces(30, ' ');

            // display timers
            for (auto& [name, timer] : _m_timers)
            {
                buffer = name + ": " + std::to_string(timer.totalTime) + "ms (" + std::to_string(timer.count) + " calls)";
                _m_window->WriteText(spaces, 2, y);
                _m_window->WriteText(buffer, 2, y++);

                // reset timer data after displaying
                timer.totalTime = 0.0f;
                timer.count = 0;
            }

            // display counters
            for (auto& [name, count] : _m_counters)
            {
                buffer = name + ": " + std::to_string(count);

                _m_window->WriteText(spaces, 2, y);  
                _m_window->WriteText(buffer, 2, y++);


                count = 0;
            }
        }

    private:
        CGXProfiler() = default;

        static inline CGXProfiler* _s_instance = nullptr;
        ConsoleGraphX::CrossProcessWindow* _m_window = nullptr;
        struct TimerData
        {
            double totalTime = 0.0;
            int count = 0;
        };

        std::unordered_map<std::string, TimerData> _m_timers;
        std::unordered_map<std::string, int> _m_counters;
    };

    class Timer
    {
    public:
        Timer(std::string_view name)
            : _name(name), _start(std::chrono::high_resolution_clock::now()) {}

        ~Timer()
        {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration = end - _start;
            CGXProfiler::Instance().AddTime(std::string(_name), duration.count());
        }

    private:
        std::string_view _name;
        std::chrono::time_point<std::chrono::high_resolution_clock> _start;
    };


}
