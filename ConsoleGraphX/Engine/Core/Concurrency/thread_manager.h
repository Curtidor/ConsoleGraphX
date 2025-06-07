#pragma once

#include <thread>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <string>
#include <atomic>
#include <vector>

namespace ConsoleGraphX_Internal 
{

    class ThreadManager 
    {
    public:
        using ThreadID = std::thread::id;
        using ThreadFunction = std::function<void(std::atomic<bool>& shouldQuit)>;

        ThreadManager();
        ~ThreadManager();

        ThreadID StartThread(const std::string& name, ThreadFunction task);
        void RequestShutdownAll();
        void JoinAll();
        bool IsShutdownRequested() const;

    private:
        struct ThreadInfo {
            std::string name;
            std::thread thread;
            std::shared_ptr<std::atomic<bool>> shouldQuit;

            ThreadInfo(std::string name, std::thread&& thread, std::shared_ptr<std::atomic<bool>> quit)
                : name(std::move(name)), thread(std::move(thread)), shouldQuit(std::move(quit)) {}
        };





        std::unordered_map<ThreadID, ThreadInfo> _m_threads;
        std::mutex _m_threadMutex;
        std::atomic<bool> _m_shutdownRequested = false;
    };

} 
