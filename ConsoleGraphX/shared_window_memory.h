#pragma once
#include "CGXPCH.h" // Ensure the necessary headers are included
#include <bitset>

// Implemented in header as other projects use this code
struct SharedWindowMemory
{
    size_t m_bufferSize;
    std::bitset<255> m_pressedKeys;

    CHAR_INFO m_buffer[]; // Actual buffer will be allocated with the shared memory

    SharedWindowMemory(size_t bufferSize) : m_bufferSize(bufferSize)
    {
        // m_buffer will start immediately after SharedWindowMemory in shared memory
    }

    ~SharedWindowMemory() = default;
};
