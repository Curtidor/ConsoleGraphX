#pragma once
#include <conio.h>
#include <algorithm>
#include <cctype>
#include <windows.h>
#include "vector2.h"
#include "screen.h"

enum class Key : char
{
    Space = 32,
    ExclamationMark = 33,
    DoubleQuote = 34,
    Hash = 35,
    Dollar = 36,
    Percent = 37,
    Ampersand = 38,
    SingleQuote = 39,
    OpenParenthesis = 40,
    CloseParenthesis = 41,
    Asterisk = 42,
    Plus = 43,
    Comma = 44,
    Minus = 45,
    Period = 46,
    Slash = 47,
    Zero = 48,
    One = 49,
    Two = 50,
    Three = 51,
    Four = 52,
    Five = 53,
    Six = 54,
    Seven = 55,
    Eight = 56,
    Nine = 57,
    Colon = 58,
    Semicolon = 59,
    LessThan = 60,
    Equals = 61,
    GreaterThan = 62,
    QuestionMark = 63,
    At = 64,
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    OpenBracket = 91,
    Backslash = 92,
    CloseBracket = 93,
    Caret = 94,
    Underscore = 95,
    Backtick = 96,
    OpenBrace = 123,
    Pipe = 124,
    CloseBrace = 125,
    Tilde = 126,
    Delete = 127
};

class InputSystem
{
private:
    static bool keys[255];
    static Vector2 mousePos;
    static char GetKey();
    static bool KeyPressed();

public:
    static void GetPressedKeys();
    static bool IsKeyPressed(Key key);
    static void UpdateMousePosition();
    const static Vector2 GetMousePosition();
};