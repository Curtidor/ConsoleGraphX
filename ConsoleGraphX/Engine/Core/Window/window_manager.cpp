#include "PCH_CGX.h"
#include <thread>
#include <span>
#include <memory>
#include "Engine\Core\Window\window_manager.h"
#include "Engine\Core\Logger\logger_manager.h"

namespace ConsoleGraphX
{
    void WindowManager::Initialize()
    {
        assert(!_s_instance);
        _s_instance = new WindowManager();
    }

    WindowManager& WindowManager::Instance()
    {
        assert(_s_instance);
        return *_s_instance;
    }

    void WindowManager::ShutDown()
    {
        _s_instance->DestroyAllWindows();
        delete _s_instance;
        _s_instance = nullptr;
    }

    void WindowManager::RegisterWindow(std::shared_ptr<AbstractWindow> window)
    {
        bool successfullyOpened = false;

        for (int retries = 0; retries < 50; ++retries)
        {
            if (window->OpenCloseEvent())
            {
                HANDLE closeEventHandle = window->GetCloseEventHandle();
                _m_windowHandleEntries.emplace_back(closeEventHandle, window);

                ConsoleGraphX_Internal::LoggerManager::Instance().LogMessage(
                    "WindowManager",
                    "Successfully opened event for window: " + window->GetWindowNameR(),
                    ConsoleGraphX_Internal::LoggerManager::LogLevel::CGX_INFO);

                successfullyOpened = true;
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (!successfullyOpened)
        {
            throw WindowEventException("Failed to open close event for window: " + window->GetWindowNameR());
        }

        OnWindowRegister.InvokeNFC(window);
        _m_windows[window->GetWindowNameR()] = window;
    }

    void WindowManager::DeregisterWindow(const std::string& windowName)
    {
        auto it = _m_windows.find(windowName);
        if (it != _m_windows.end())
        {
            HANDLE handleToRemove = it->second->GetCloseEventHandle();

            _m_windowHandleEntries.erase(
                std::remove_if(_m_windowHandleEntries.begin(), _m_windowHandleEntries.end(),
                    [handleToRemove](const WindowHandleEntry& entry) {
                        return entry.handle == handleToRemove;
                    }),
                _m_windowHandleEntries.end());

            OnWindowDeregister.InvokeNFC(it->second);
            it->second->Destroy();
            _m_windows.erase(it);
        }
    }

    void WindowManager::MonitorWindowCloses(ConsoleGraphX_Internal::ThreadManager& threadManager)
    {
        threadManager.StartThread("WindowCloseMonitor", [this](std::atomic<bool>& shouldQuit)
            {
                std::vector<HANDLE> handles;

                while (!shouldQuit.load(std::memory_order_acquire))
                {
                    if (handles.size() != _m_windowHandleEntries.size()) {
                        UpdateHandles(handles);
                    }

                    if (handles.empty()) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        continue;
                    }

                    DWORD waitResult = WaitForMultipleObjects(
                        static_cast<DWORD>(handles.size()),
                        handles.data(),
                        FALSE,
                        100);

                    if (waitResult >= WAIT_OBJECT_0 && waitResult < WAIT_OBJECT_0 + handles.size()) {
                        int index = waitResult - WAIT_OBJECT_0;
                        const HANDLE triggeredHandle = handles[index];

                        auto it = std::find_if(
                            _m_windowHandleEntries.begin(),
                            _m_windowHandleEntries.end(),
                            [triggeredHandle](const WindowHandleEntry& entry) {
                                return entry.handle == triggeredHandle;
                            });

                        if (it != _m_windowHandleEntries.end()) {
                            if (auto closedWindow = it->window.lock())
                            {
                                ConsoleGraphX_Internal::LoggerManager::Instance().LogMessage(
                                    "WindowManager",
                                    "Window closed: " + closedWindow->GetWindowNameR(),
                                    ConsoleGraphX_Internal::LoggerManager::LogLevel::CGX_INFO);

                                //closedWindow->OnWindowClosed.InvokeNFC(closedWindow.get());
                                _m_windowsToClose.push_back(closedWindow->GetWindowNameR());
                            }

                            ResetEvent(triggeredHandle);
                        }
                    }
                }
            });
    }

    void WindowManager::ProcessToCloseWindows()
    {
        for (const std::string& windowName : _m_windowsToClose)
        {
            DeregisterWindow(windowName);
        }
        _m_windowsToClose.clear();
    }

    std::shared_ptr<AbstractWindow> WindowManager::GetSharedWindow(const std::string& windowName)
    {
        auto it = _m_windows.find(windowName);
        return (it != _m_windows.end()) ? it->second : nullptr;
    }

    std::vector<std::shared_ptr<AbstractWindow>> WindowManager::GetAllSharedWindows() const
    {
        std::vector<std::shared_ptr<AbstractWindow>> result;
        result.reserve(_m_windows.size());

        for (const auto& [_, window] : _m_windows)
            result.push_back(window);

        return result;
    }

    void WindowManager::DestroyAllWindows()
    {
        for (auto& [_, window] : _m_windows)
            window->Destroy();

        _m_windows.clear();
    }

    void WindowManager::UpdateHandles(std::vector<HANDLE>& handles)
    {
        handles.clear();
        for (const auto& entry : _m_windowHandleEntries)
            handles.push_back(entry.handle);
    }
}
