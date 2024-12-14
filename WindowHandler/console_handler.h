#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <functional>

class ConsoleHandler
{
public:
    /**
     * @brief Sets the console control handler to intercept close events.
     */
    static void SetHandler();

    /**
     * @brief Registers a callback function to be executed on console close events.
     * @param callback The callback function to be executed.
     */
    static void RegisterCloseCallback(const std::function<void()>& callback);

private:
    // Static member to store the callback function
    static std::function<void()> _m_callback;

    /**
     * @brief Internal console control handler routine.
     * @param ctrlType The type of control signal received.
     * @return TRUE if the event was handled; otherwise FALSE.
     */
    static BOOL WINAPI _ConsoleHandlerRoutine(DWORD ctrlType);
};

