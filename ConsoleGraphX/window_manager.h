#pragma once
#include <unordered_map>
#include <string>
#include <thread>
#include "events.h"
#include "window.h"
#include "window_layout.h"

namespace ConsoleGraphX
{
	class WindowManager
	{
	public:
		CGXEventArgs<AbstractWindow*> OnWindowCreate;
		CGXEventArgs<AbstractWindow*> OnWindowRegister;
		CGXEventArgs<AbstractWindow*> OnWindowDeregister;

	public:
		static void Initialize();
		static WindowManager& Instance();
		static void ShutDown();

		void RegisterWindow(std::unique_ptr<AbstractWindow> window);
		void DeregisterWindow(const std::string& windowName);

		CrossProcessWindow* CreateCGXWindow(short width, short height, short fontWidth, short fontHeight, const char* name);
		AbstractWindow* GetWindow(const std::string& windowName);


	private:
		std::unordered_map<std::string, std::unique_ptr<AbstractWindow>> _m_windows;

		static inline WindowManager* _s_instance = nullptr; 

	private:
		void DestroyAllWindows();
	};

}
