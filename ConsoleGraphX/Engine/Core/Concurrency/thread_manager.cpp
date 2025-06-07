#include "PCH_CGX.h"
#include <unordered_map>
#include "thread_manager.h"
#include <cassert>

namespace ConsoleGraphX_Internal
{

    ThreadManager::ThreadManager() = default;

    ThreadManager::~ThreadManager() 
    {
    #if MIN_BUILD == 0
        RequestShutdownAll();
        JoinAll();
    #endif
    }

    ThreadManager::ThreadID ThreadManager::StartThread(const std::string& name, ThreadFunction task)
    {
        std::shared_ptr<std::atomic<bool>> quitFlag = std::make_shared<std::atomic<bool>>(false);

        std::thread t([quitFlag, task]() {
            task(*quitFlag);  // pass by reference
            });

        ThreadID tid = t.get_id();

        {
            std::lock_guard<std::mutex> lock(_m_threadMutex);
            _m_threads.try_emplace(
                tid,
                name,
                std::move(t),
                std::move(quitFlag)
            );
        }

        return tid;
    }



    void ThreadManager::RequestShutdownAll() 
    {
        _m_shutdownRequested = true;

        std::lock_guard<std::mutex> lock(_m_threadMutex);
        for (auto& [id, info] : _m_threads) {
            info.shouldQuit->store(true);
        }
    }

    void ThreadManager::JoinAll() 
    {
        std::lock_guard<std::mutex> lock(_m_threadMutex);

        for (auto& [id, info] : _m_threads) {
            if (info.thread.joinable()) {
                info.thread.join();
            }
        }
        _m_threads.clear();
    }

    bool ThreadManager::IsShutdownRequested() const 
    {
        return _m_shutdownRequested.load();
    }

}
