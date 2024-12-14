#include "PCH_CGX.h"
#include <thread>
#include <span>
#include "window_manager.h"
#include "logger_manager.h"


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
    }

    void WindowManager::RegisterWindow(std::unique_ptr<AbstractWindow> window)
    {
        bool successfullyOpened = false;

        for (int retries = 0; retries < 50; ++retries)
        {
            if (window->OpenCloseEvent())
            {
                HANDLE closeEventHandle = window->GetCloseEventHandle();
                _m_windowHandleEntries.emplace_back(closeEventHandle, window.get());

                ConsoleGraphX_Internal::LoggerManager::Instance().LogMessage(
                    "WindowManager",
                    "Successfully opened event for window: " + window->GetWindowNameR(),
                    ConsoleGraphX_Internal::LoggerManager::LogLevel::CGX_INFO);

                successfullyOpened = true;
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // retry after a short delay
        }

        if (!successfullyOpened)
        {
            ConsoleGraphX_Internal::LoggerManager::Instance().LogMessage(
                "WindowManager",
                "Failed to open event for window: " + window->GetWindowNameR(),
                ConsoleGraphX_Internal::LoggerManager::LogLevel::CGX_ERROR);

            // if we exhaust retries, throw an exception
            throw WindowEventException("Failed to open close event for window: " + window->GetWindowNameR());
        }

        OnWindowRegister.InvokeNFC(window.get());
        _m_windows.insert({ window->GetWindowNameR(), std::move(window) });
    }


    void WindowManager::DeregisterWindow(const std::string& windowName)
    {
        auto it = _m_windows.find(windowName);
        if (it != _m_windows.end())
        {
            HANDLE handleToRemove = it->second->GetCloseEventHandle();

            // remove the entry from the vector
            _m_windowHandleEntries.erase(
                std::remove_if(_m_windowHandleEntries.begin(), _m_windowHandleEntries.end(),
                    [handleToRemove](const WindowHandleEntry& entry) {
                        return entry.handle == handleToRemove;
                    }),
                _m_windowHandleEntries.end());

            OnWindowDeregister.InvokeNFC(it->second.get());

            it->second.get()->Destroy();

            _m_windows.erase(it);
        }
    }

    void WindowManager::MonitorWindowCloses()
    {
        // Welcome to the sketchy code (its all sketchy)
        std::thread([this]()
            {
                std::vector<HANDLE> handles; 

                while (true)
                {
                    if (handles.size() != _m_windowHandleEntries.size())
                    {
                        UpdateHandles(handles); // update the handles only when necessary
                    }

                    if (handles.empty())
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        continue;
                    }

                    DWORD waitResult = WaitForMultipleObjects(
                        static_cast<DWORD>(handles.size()),
                        handles.data(),
                        FALSE,
                        100);

                    if (waitResult >= WAIT_OBJECT_0 && waitResult < WAIT_OBJECT_0 + handles.size())
                    {
                        int signaledIndex = waitResult - WAIT_OBJECT_0;
                        const HANDLE triggeredHandle = handles[signaledIndex];

                        // find the corresponding window
                        auto it = std::find_if(
                            _m_windowHandleEntries.begin(),
                            _m_windowHandleEntries.end(),
                            [triggeredHandle](const WindowHandleEntry& entry) 
                            {
                                return entry.handle == triggeredHandle;
                            });

                        if (it != _m_windowHandleEntries.end())
                        {
                            AbstractWindow* closedWindow = it->window;

                            ConsoleGraphX_Internal::LoggerManager::Instance().LogMessage(
                                "WindowManager",
                                "Window closed: " + closedWindow->GetWindowNameR(),
                                ConsoleGraphX_Internal::LoggerManager::LogLevel::CGX_INFO);

                            closedWindow->OnWindowClosed.InvokeNF(closedWindow);
                            _m_windowsToClose.push_back(closedWindow->GetWindowNameR());
                        }

                        ResetEvent(triggeredHandle);
                    }

                    //std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }).detach();
    }


    void WindowManager::ProcessToCloseWindows()
    {
        for (const std::string& windowName : _m_windowsToClose)
        {
            DeregisterWindow(windowName);
        }

        _m_windowsToClose.clear();
    }

    AbstractWindow* WindowManager::GetWindow(const std::string& windowName)
    {
        auto it = _m_windows.find(windowName);

        if (it != _m_windows.end())
            return it->second.get();

        return nullptr;
    }

    std::vector<AbstractWindow*> WindowManager::GetAllWindows() const
    {
        std::vector<AbstractWindow*> windows;
        windows.reserve(_m_windows.size());

        for (const auto& pair : _m_windows)
        {
            windows.emplace_back(pair.second.get());
        }

        return windows; // double allocation :/ eh it's non crit code
    }

    void WindowManager::DestroyAllWindows()
    {
        for (auto& [name, window] : _m_windows)
        {
            window->Destroy();
        }
        _m_windows.clear();
    }
    
    void WindowManager::UpdateHandles(std::vector<HANDLE>& handles)
    {
        handles.clear();
        for (const auto& entry : _m_windowHandleEntries)
        {
            handles.push_back(entry.handle);
        }
    }

};
