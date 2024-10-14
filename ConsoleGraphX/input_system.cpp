#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <consoleapi2.h>
#include <wincontypes.h>
#include <processenv.h>
#include <algorithm> // false postive algo is used
#include <cctype>
#include <conio.h>
#include <consoleapi.h>
#include "vector2.h"
#include "input_system.h"

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

    void InputSystem::HandleKeyEvent(const KEY_EVENT_RECORD& keyEvent)
    {
        if (keyEvent.bKeyDown)
        {
            InputSystem::keys[keyEvent.wVirtualKeyCode] = true;
        }
        else
        {
            InputSystem::keys[keyEvent.wVirtualKeyCode] = false;
        }
    }


    void InputSystem::ProcessInput()
    {
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        INPUT_RECORD irInBuf[128];
        DWORD cNumRead = 0;

        if (!PeekConsoleInput(hStdin, irInBuf, 128, &cNumRead) || cNumRead == 0)
        {
            return; // no input events to process
        }

        // dispatch the events to the appropriate handler
        for (DWORD i = 0; i < cNumRead; i++)
        {
            switch (irInBuf[i].EventType)
            {
            case KEY_EVENT:
                HandleKeyEvent(irInBuf[i].Event.KeyEvent);
                break;
            case MOUSE_EVENT:
                HandleMouseEvent(irInBuf[i].Event.MouseEvent);
                break;
            default:
                break;
            }
        }

        FlushConsoleInputBuffer(hStdin);
    }


};