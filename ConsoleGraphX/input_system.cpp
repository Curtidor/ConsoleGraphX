#include "input_system.h"


bool InputSystem::keys[255];
Vector2 InputSystem::mousePos;

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

    // Access the keys array using the lowercase key
    return InputSystem::keys[static_cast<int>(lowercaseKey)] || InputSystem::keys[static_cast<int>(key)];
}

/**
 * Update the mouse position based on the current cursor position.
 */
void InputSystem::UpdateMousePosition()
{
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(Screen::GetActiveScreen_A()->GetConsoleWindowHandle(), &cursorPos);

    InputSystem::mousePos.x = cursorPos.x;
    InputSystem::mousePos.y = cursorPos.y;
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
