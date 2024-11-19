#include "PCH_CGX.h"
#include <thread>
#include "window_manager.h"

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

    void WindowManager::RegisterWindow(std::unique_ptr<ConsoleGraphX::AbstractWindow> window)
    {
        //TODO: FIX HARDCODED SLEEP
        // this is called before the window is actually up so we have to wait a bit
        // this is a temp solution and should be fixed soon
        Sleep(50);

        OnWindowRegister.InvokeNFC(window.get());

        _m_windows.insert({ window->GetWindowNameR(), std::move(window) });
    }

    void WindowManager::DeregisterWindow(const std::string& windowName)
    {
        auto it = _m_windows.find(windowName);
        if (it != _m_windows.end())
        {
            OnWindowDeregister.InvokeNFC(it->second.get());
            _m_windows.erase(it);
        }
    }

    // This method only creates cross process windows
    // NOTE: This method automatically registers the window
    ConsoleGraphX::CrossProcessWindow* WindowManager::CreateCGXWindow(short width, short height, short fontWidth, short fontHeight, const char* name)
    {
        std::unique_ptr<CrossProcessWindow> newWindow = std::make_unique<CrossProcessWindow>(width, height, fontWidth, fontHeight, name, nullptr);

        newWindow->CreateConsoleWindow();

        OnWindowCreate.InvokeNFC(newWindow.get());

        RegisterWindow(std::move(newWindow));

        return static_cast<CrossProcessWindow*>(GetWindow(name));
    }

    AbstractWindow* WindowManager::GetWindow(const std::string& windowName)
    {
        auto itUser = _m_windows.find(windowName);
        if (itUser != _m_windows.end())
            return itUser->second.get();

        return nullptr;
    }

    void WindowManager::DestroyAllWindows()
    {
        for (auto& [name, window] : _m_windows)
        {
            window->Destroy();
        }
        _m_windows.clear();
    }
};
