#pragma once
#include <windows.h>
#include <memoryapi.h>
#include <synchapi.h>
#include <handleapi.h>
#include <stdexcept>
#include <type_traits>
#include <string>
#include "ipc_base.h"
#include "shared_memory.h"

template <typename T>
class Sender : public IPCBase<T> {
public:
    Sender(const std::wstring& identifier) : IPCBase<T>(identifier) {

        this->_m_hMutex = CreateMutex(NULL, FALSE, this->_m_identifierMutex.c_str());
        if (this->_m_hMutex == NULL) {
            throw std::runtime_error("Failed to create mutex handle. Error: " + GetLastError());
        }

        this->_m_hEvent = CreateEvent(NULL, FALSE , FALSE, this->_m_identifierEvent.c_str());
        if (this->_m_hEvent == NULL) {
            CloseHandle(this->_m_hMutex);
            throw std::runtime_error("Failed to create event handle. Error: " + GetLastError());
        }

        this->_m_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedMemory<T>), this->_m_identifierMapFile.c_str());
        if (this->_m_hMapFile == NULL) {
            CloseHandle(this->_m_hEvent);
            CloseHandle(this->_m_hMutex);
            throw std::runtime_error("Failed to create file mapping handle. Error: " + GetLastError());
        }

        this->_m_pSharedMemory.reset((SharedMemory<T>*)MapViewOfFile(this->_m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory<T>)));
        if (this->_m_pSharedMemory == nullptr) {
            CloseHandle(this->_m_hMapFile);
            CloseHandle(this->_m_hEvent);
            CloseHandle(this->_m_hMutex);
            throw std::runtime_error("Failed to map view of file. Error: " + GetLastError());
        }
    }

    void SendMessageIPC(const T& data) {
        WaitForSingleObject(this->_m_hMutex, INFINITE);

        if constexpr (std::is_same<T, std::string>::value) {
            strncpy_s(this->_m_pSharedMemory->data, data.c_str(), BUFFER_SIZE);
        }
        else {
            this->_m_pSharedMemory->data = data;
        }

        ReleaseMutex(this->_m_hMutex);
        SetEvent(this->_m_hEvent);
    }
};
