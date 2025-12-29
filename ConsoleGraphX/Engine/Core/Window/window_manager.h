#pragma once

#include <unordered_map>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <memory>
#include "Engine\Core\Event\events.h"
#include "Engine\Core\Window\window.h"
#include "Engine\Core\Window\Layout\window_layout.h"
#include "Engine\Core\Concurrency\thread_manager.h"

namespace ConsoleGraphX
{
	struct WindowHandleEntry
	{
		HANDLE handle;
		std::weak_ptr<AbstractWindow> window;

		WindowHandleEntry(HANDLE h, std::weak_ptr<AbstractWindow> w) : handle(h), window(std::move(w)) {}
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
		CGXEventArgs<std::shared_ptr<AbstractWindow>> OnWindowCreate;
		CGXEventArgs<std::shared_ptr<AbstractWindow>> OnWindowRegister;
		CGXEventArgs<std::shared_ptr<AbstractWindow>> OnWindowDeregister;

	public:
		static void Initialize();
		static WindowManager& Instance();
		static void ShutDown();

		void RegisterWindow(std::shared_ptr<AbstractWindow> window);
		void DeregisterWindow(const std::string& windowName);
		void MonitorWindowCloses(ConsoleGraphX_Internal::ThreadManager& threadManager);
		void ProcessToCloseWindows();

		template <typename WindowType>
		std::shared_ptr<WindowType> CreateCGXWindow(short width, short height, short fontWidth, short fontHeight, const char* name)
		{
			static_assert(std::is_base_of_v<AbstractWindow, WindowType>, "WindowType must inherit AbstractWindow");

			std::shared_ptr<WindowType> newWindow;

			if constexpr (std::is_same_v<Window, WindowType>)
			{
				newWindow = std::make_shared<Window>(width, height, fontWidth, fontHeight, name);
			}
			else
			{
				newWindow = std::make_shared<WindowType>(width, height, fontWidth, fontHeight, name, nullptr);
			}

			newWindow->SetupWindow();

			OnWindowCreate.InvokeNFC(newWindow);

			RegisterWindow(newWindow);

			return newWindow;
		}

		std::shared_ptr<AbstractWindow> GetSharedWindow(const std::string& windowName);
		std::vector<std::shared_ptr<AbstractWindow>> GetAllSharedWindows() const;

		std::vector< std::shared_ptr<CrossProcessWindow>> GetAllCrossProcessWindows() const;

	private:
		static inline WindowManager* _s_instance = nullptr;

		std::unordered_map<std::string, std::shared_ptr<AbstractWindow>> _m_windows;
		std::vector<WindowHandleEntry> _m_windowHandleEntries;
		std::vector<std::string> _m_windowsToClose;
		std::atomic<bool> _m_wantsToQuit{ false };

	private:
		void UpdateHandles(std::vector<HANDLE>& handles);
		void HandleShrunkenWindow(std::shared_ptr<AbstractWindow> window);
		void DestroyAllWindows();
	};
}
