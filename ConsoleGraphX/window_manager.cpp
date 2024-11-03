#include "PCH_CGX.h"
#include <thread>
#include "window_manager.h"
#include "window_factory.h"

namespace ConsoleGraphX
{
    WindowManager::WindowManager()
    {
    }

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

    void WindowManager::RegisterWindow(std::unique_ptr<ConsoleGraphX::Window> window)
    {
        // this is called before the window is actually up so we have to wait a bit
        // this is a temp solution and should be fixed soon
        // TODO
        Sleep(50);

        OnWindowRegister.InvokeNFC(window.get());

        if (window->GetType() == ConsoleGraphX::WindowType::UserCreated)
        {
            _m_windowsUser.insert({ window->GetWindowNameR(), std::move(window) });
        }
        else
        {
            _m_windowsUser.insert({ window->GetWindowNameR(), std::move(window) });
        }
    }

    void WindowManager::DeregisterWindow(ConsoleGraphX::Window* window)
    {
        if (window->GetType() == ConsoleGraphX::WindowType::UserCreated)
        {
            auto it = _m_windowsUser.find(window->GetWindowNameR());
            if (it != _m_windowsUser.end())
            {
                OnWindowDeregister.InvokeNFC(it->second.get());  
                _m_windowsUser.erase(it);
            }
        }
        else
        {
            auto it = _m_windowsEngine.find(window->GetWindowNameR());
            if (it != _m_windowsEngine.end())
            {
                OnWindowDeregister.InvokeNFC(it->second.get()); 
                _m_windowsEngine.erase(it);
            }
        }
    }

    ConsoleGraphX::Window* WindowManager::CreateCGXWindow(short width, short height, short fontWidth, short fontHeight, const char* name, ConsoleGraphX::WindowType windowType)
    {
        std::unique_ptr<Window> newWindow;

        if (windowType == ConsoleGraphX::WindowType::UserCreated)
        {
            newWindow = ConsoleGraphX_Internal::CreateUserWindow(width, height, name, fontWidth, fontHeight);
        }
        else
        {
            newWindow = ConsoleGraphX_Internal::CreateEngineWindow(width, height, name, fontWidth, fontHeight);
        }

        newWindow->CreateConsoleWindow();

        OnWindowCreate.InvokeNFC(newWindow.get());

        RegisterWindow(std::move(newWindow));

        return GetWindow(name);
    }

    Window* WindowManager::GetWindow(const std::string& windowName)
    {
        auto itUser = _m_windowsUser.find(windowName);
        if (itUser != _m_windowsUser.end())
            return itUser->second.get();

        auto itEngine = _m_windowsEngine.find(windowName);
        if (itEngine != _m_windowsEngine.end())
            return itEngine->second.get();

        return nullptr;
    }

    void WindowManager::DestroyAllWindows()
    {
        for (auto& [name, window] : _m_windowsUser)
        {
            window->Destroy();
        }
        _m_windowsUser.clear();

        for (auto& [name, window] : _m_windowsEngine)
        {
            window->Destroy();
        }
        _m_windowsEngine.clear();
    }


};
