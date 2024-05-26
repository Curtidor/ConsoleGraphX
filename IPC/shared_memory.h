#pragma once
#include <string>

const int BUFFER_SIZE = 4069;

template <typename T>
struct SharedMemory {
    T data;
};

template<>
struct SharedMemory<std::string> {
    char data[BUFFER_SIZE];
};
