#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincontypes.h>
#include <processenv.h>
#include <cctype>
#include <conio.h>
#include <algorithm>
#include "input_system.h"
#include "vector2.h"

namespace ConsoleGraphX
{
    bool InputSystem::keys[255];
    Vector2 InputSystem::mousePos;
    bool InputSystem::leftMouseButtonDown = false;
    bool InputSystem::rightMouseButtonDown = false;

    /**
     * Get the next character from the input buffer if available, otherwise return '\0'.
     *
     * @return The next character from the input buffer if available, '\0' otherwise.
     */
    char InputSystem::GetKey()
    {
        if (_kbhit())
        {
            return _getch();
        }
        else
        {
            return '\0';
        }
    }

    /**
     * Get all the currently pressed keys and update the keys array accordingly.
     */
    void InputSystem::GetPressedKeys()
    {
        // Set all the values in the array to false
        std::fill(std::begin(InputSystem::keys), std::end(InputSystem::keys), false);

        while (KeyPressed())
        {
            // Get the pressed key
            char key = InputSystem::GetKey();
            InputSystem::keys[key] = true;
        }
    }

    /**
     * Check if any key is currently being pressed.
     *
     * @return True if a key is being pressed, false otherwise.
     */
    bool InputSystem::KeyPressed()
    {
        return _kbhit() != 0;
    }

    /**
     * Check if the specified key is currently being pressed.
     *
     * @param key The key to check.
     * @return True if the key is being pressed, false otherwise.
     */
    bool InputSystem::IsKeyPressed(Key key)
    {
        // Convert the key value to lowercase
        char lowercaseKey = std::tolower(static_cast<char>(key));

        return InputSystem::keys[static_cast<int>(lowercaseKey)] || InputSystem::keys[static_cast<int>(key)];
    }


    /**
     * Get the current mouse position.
     *
     * @return The current mouse position relative to the console window.
     */
    const Vector2 InputSystem::GetMousePosition()
    {
        return InputSystem::mousePos;
    }
    #pragma warning (disable: VCIC001)
    void InputSystem::HandleMouseEvent(const MOUSE_EVENT_RECORD& mouseEvent)
    {
        switch (mouseEvent.dwEventFlags)
        {
        case 0: // Button press/release events
            if (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)
            {
                leftMouseButtonDown = true;
            }
            else
            {
                leftMouseButtonDown = false;
            }

            if (mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED)
            {
                rightMouseButtonDown = true;
            }
            else
            {
                rightMouseButtonDown = false;
            }
            break;

        case MOUSE_MOVED:
            mousePos.x = mouseEvent.dwMousePosition.X;
            mousePos.y = mouseEvent.dwMousePosition.Y;
            break;
        }
    }

    void InputSystem::ProcessInput()
    {
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        DWORD cNumRead = 0;
        DWORD i = 0;
        INPUT_RECORD irInBuf[128];

        DWORD x = 0;

        // Read input events
        GetNumberOfConsoleInputEvents(hStdin, &x);
        if (x)
            ReadConsoleInput(hStdin, irInBuf, 128, &cNumRead);


        // Dispatch the events to the appropriate handler
        for (i = 0; i < cNumRead; i++)
        {
            switch (irInBuf[i].EventType)
            {
            case KEY_EVENT: // keyboard input
                // Handle keyboard events if necessary
                break;

            case MOUSE_EVENT: // mouse input
                HandleMouseEvent(irInBuf[i].Event.MouseEvent);
                break;

            default:
                break;
            }
        }
    }

};