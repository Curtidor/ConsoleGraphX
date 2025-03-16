#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bitset>

// WARNING: This structure is intended to be used **only** in the Window class and WindowHandler program for a very specific use case.
// It is tightly coupled to the shared memory design of the windows and should not be reused elsewhere.
struct SharedWindowMemory
{
    size_t m_bufferSize;
    std::bitset<255> m_pressedKeys;

    // change to a one-element array, with the rest allocated later
    CHAR_INFO m_buffer[1];  // flexible array, actual size determined at runtime

    // disable the warning for uninitialized members specifically for this structure
#pragma warning(push)
#pragma warning(disable : 26495)

    SharedWindowMemory(size_t bufferSize) : m_bufferSize(bufferSize) {}

    ~SharedWindowMemory() = default;

#pragma warning(pop)
};
