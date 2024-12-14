#pragma once
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include "events.h"
#include "window.h"
#include "window_layout.h"

namespace ConsoleGraphX
{

	struct WindowHandleEntry
	{
		HANDLE handle;
		AbstractWindow* window;

		WindowHandleEntry(HANDLE h, AbstractWindow* w) : handle(h), window(w) {}
	};

	class WindowEventException : public std::runtime_error
	{
	public:
		explicit WindowEventException(const std::string& message)
			: std::runtime_error("WindowEventException: " + message) {}
	};


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
		void MonitorWindowCloses();
		void ProcessToCloseWindows();

		template <typename WindowType>
		WindowType* CreateCGXWindow(short width, short height, short fontWidth, short fontHeight, const char* name)
		{
			static_assert(std::is_base_of_v<AbstractWindow, WindowType>, "WindowType Must be of derived from AbstractWindow");

			std::unique_ptr<WindowType> newWindow;

			if constexpr (std::is_same_v<Window, WindowType>)
			{
				newWindow = std::make_unique<WindowType>(width, height, fontWidth, fontHeight, name);
			}
			else
			{
				newWindow = std::make_unique<WindowType>(width, height, fontWidth, fontHeight, name, nullptr);
			}

			newWindow->SetupWindow();

			OnWindowCreate.InvokeNFC(newWindow.get());

			RegisterWindow(std::move(newWindow));

			return static_cast<WindowType*>(GetWindow(name));
		}
		
		AbstractWindow* GetWindow(const std::string& windowName);
		std::vector<AbstractWindow*> GetAllWindows() const;

	private:
		static inline WindowManager* _s_instance = nullptr;

		std::unordered_map<std::string, std::unique_ptr<AbstractWindow>> _m_windows;
		std::vector<WindowHandleEntry> _m_windowHandleEntries;
		std::vector<std::string> _m_windowsToClose;

	private:
		void UpdateHandles(std::vector<HANDLE>& handles);
		void DestroyAllWindows();
	};
}
