#include "PCH_CGX.h"
#include <conio.h>
#include "Engine/Systems/input_system.h"

namespace ConsoleGraphX
{
    InputSystem* InputSystem::_s_instance = nullptr;  

    void InputSystem::Initialize()
    {
        assert(!_s_instance);
        _s_instance = new InputSystem();
    }

    InputSystem& InputSystem::Instance()
    {
        assert(_s_instance);
        return *_s_instance;
    }

    void InputSystem::ShutDown()
    {
        delete _s_instance;
        _s_instance = nullptr;
    }

    int InputSystem::GetKey()
    {
        if (_kbhit()) return _getch();
        return '\0';
    }

    void InputSystem::GetPressedKeys()
    {
        std::fill(std::begin(_s_instance->_m_keys), std::end(_s_instance->_m_keys), false);
        while (KeyPressed())
        {
            char key = GetKey();
            _s_instance->_m_keys[key] = true;
        }
    }

    bool InputSystem::KeyPressed()
    {
        return _kbhit() != 0;
    }

    bool InputSystem::IsKeyPressed(Key key)
    {
        char lowercaseKey = std::tolower(static_cast<char>(key));
        return _s_instance->_m_keys[static_cast<int>(lowercaseKey)] || _s_instance->_m_keys[static_cast<int>(key)];
    }

    const Vector2 InputSystem::GetMousePosition()
    {
        return _s_instance->_m_mousePos;
    }

    void InputSystem::HandleMouseEvent(const MOUSE_EVENT_RECORD& mouseEvent)
    {
        switch (mouseEvent.dwEventFlags)
        {
        case 0:
            _s_instance->_m_leftMouseButtonDown = (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED);
            _s_instance->_m_rightMouseButtonDown = (mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED);
            break;
        case MOUSE_MOVED:
            _s_instance->_m_mousePos.x = mouseEvent.dwMousePosition.X;
            _s_instance->_m_mousePos.y = mouseEvent.dwMousePosition.Y;
            break;
        }
    }

    void InputSystem::HandleKeyEvent(const KEY_EVENT_RECORD& keyEvent)
    {
        _s_instance->_m_keys[keyEvent.wVirtualKeyCode] = keyEvent.bKeyDown;
    }

    void InputSystem::ProcessInput()
    {
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        INPUT_RECORD irInBuf[128];
        DWORD cNumRead = 0;

        if (!PeekConsoleInput(hStdin, irInBuf, 128, &cNumRead) || cNumRead == 0) return;

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
            }
        }

        FlushConsoleInputBuffer(hStdin);
    }

    Input InputSystem::GetInputSnapshot()
    {
        return Input(
            _s_instance->_m_keys,
            _s_instance->_m_mousePos,
            _s_instance->_m_leftMouseButtonDown,
            _s_instance->_m_rightMouseButtonDown
        );
    }

    Input::Input(const std::array<bool, 255>& keys, Vector2 mousePos, bool leftClick, bool rightClick): 
        keyStates(keys), mousePosition(mousePos), leftMouseButton(leftClick), rightMouseButton(rightClick)
    {}

    bool Input::IsKeyPressed(Key key) const
    {
        char lowercaseKey = std::tolower(static_cast<char>(key));
        return keyStates[static_cast<int>(lowercaseKey)] || keyStates[static_cast<int>(key)];
    }

    bool Input::IsLeftMousePressed() const
    {
        return leftMouseButton;
    }

    bool Input::IsRightMousePressed() const
    {
        return rightMouseButton;
    }
}
