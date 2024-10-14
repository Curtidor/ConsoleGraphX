#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wincontypes.h>
#include <bitset>

struct SharedMemory
{
    size_t m_bufferSize;
    CHAR_INFO* m_buffer;
    std::bitset<255> m_pressedKeys;

    SharedMemory(size_t bufferSize) : m_bufferSize(bufferSize)
    {
        m_buffer = reinterpret_cast<CHAR_INFO*>(this + 1); // Offset within the shared memory region
    }

    ~SharedMemory() = default;
};
