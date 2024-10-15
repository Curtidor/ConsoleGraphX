#pragma once
#include <unordered_map>
#include <string>
#include "events.h"
#include "window.h"

namespace ConsoleGraphX
{
	class WindowManager
	{
	public:
		CGXEventArgs<Window*> OnWindowCreate;
		CGXEventArgs<Window*> OnWindowRegister;
		CGXEventArgs<Window*> OnWindowDeregister;

	public:
		static void Initialize();
		static WindowManager& Instance();
		static void ShutDown();

		void RegisterWindow(std::unique_ptr<Window> window);
		void DeregisterWindow(Window* window);

		Window* CreateCGXWindow(short width, short height, short fontWidth, short fontHeight, const char* name, WindowType windowType = WindowType::UserCreated);


	private:
		std::unordered_map<std::string, std::unique_ptr<Window>> _m_windowsUser;
		std::unordered_map<std::string, std::unique_ptr<Window>> _m_windowsEngine;

		static inline WindowManager* _s_instance = nullptr; 
	};

}
