#include "CGXPCH.h"
#include "window_manager.h"
#include "window_factory.h"

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
        delete _s_instance;
    }
    
    void WindowManager::RegisterWindow(std::unique_ptr<ConsoleGraphX::Window> window)
    {
        OnWindowRegister.InvokeNFC(window.get());

        if (window.get()->GetType() == ConsoleGraphX::WindowType::UserCreated)
        {
            _m_windowsUser.insert({ window.get()->GetWindowNameR(), std::move(window) });
        }
        else
        {
            _m_windowsUser.insert({ window.get()->GetWindowNameR(), std::move(window) });
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
        ConsoleGraphX::Window* newWindow;

        if (windowType == ConsoleGraphX::WindowType::UserCreated)
        {
            auto window = ConsoleGraphX_Internal::CreateUserWindow(width, height, name, fontWidth, fontHeight);
            _m_windowsUser.insert({ window.get()->GetWindowNameR(), std::move(window) });

            newWindow = _m_windowsUser[name].get();
        }
        else
        {
            auto window = ConsoleGraphX_Internal::CreateEngineWindow(width, height, name, fontWidth, fontHeight);
            _m_windowsEngine.insert({ window.get()->GetWindowNameR(), std::move(window) });

            newWindow = _m_windowsEngine[name].get();
        }

        OnWindowCreate.InvokeNF(newWindow);

        return newWindow;
    }

};
