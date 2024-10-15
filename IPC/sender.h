#pragma once
#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>
#include <memoryapi.h>
#include <synchapi.h>
#include <handleapi.h>
#include <stdexcept>
#include <type_traits>
#include <string>
#include "ipc_base.h"
#include "shared_memory.h"

template <typename T>
class Sender : public IPCBase<T> 
{
public:
    Sender(const std::wstring& identifier)
        : IPCBase<T>(identifier)
    {
        this->_m_hMutex = CreateMutex(NULL, FALSE, this->_m_identifierMutex.c_str());
        if (this->_m_hMutex == NULL) 
        {
            throw std::runtime_error("CreateMutex error: " + std::to_string(GetLastError()));
        }

        this->_m_hEventSend = CreateEvent(NULL, FALSE, FALSE, (this->_m_identifierEvent + L"Send").c_str());
        if (this->_m_hEventSend == NULL) 
        {
            CloseHandle(this->_m_hMutex);
            throw std::runtime_error("CreateEvent error: " + std::to_string(GetLastError()));
        }

        this->_m_hEventRead = CreateEvent(NULL, FALSE, FALSE, (this->_m_identifierEvent + L"Read").c_str());
        if (this->_m_hEventRead == NULL) 
        {
            CloseHandle(this->_m_hEventSend);
            CloseHandle(this->_m_hMutex);
            throw std::runtime_error("CreateEvent error: " + std::to_string(GetLastError()));
        }

        this->_m_hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedMemory<T>), this->_m_identifierMapFile.c_str());
        if (this->_m_hMapFile == NULL) 
        {
            CloseHandle(this->_m_hEventRead);
            CloseHandle(this->_m_hEventSend);
            CloseHandle(this->_m_hMutex);
            throw std::runtime_error("CreateFileMapping error: " + std::to_string(GetLastError()));
        }

        auto ptr = static_cast<SharedMemory<T>*>(MapViewOfFile(this->_m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory<T>)));
        if (ptr == nullptr) 
        {
            CloseHandle(this->_m_hMapFile);
            CloseHandle(this->_m_hEventRead);
            CloseHandle(this->_m_hEventSend);
            CloseHandle(this->_m_hMutex);
            throw std::runtime_error("MapViewOfFile error: " + std::to_string(GetLastError()));
        }
        this->_m_pSharedMemory.reset(ptr);
    }

    void SendMessageIPC(const T& data) 
    {
        DWORD dwWaitResult = WaitForSingleObject(this->_m_hMutex, INFINITE);
        if (dwWaitResult == WAIT_OBJECT_0) {

            if constexpr (std::is_same<T, std::string>::value) {
                strncpy_s(this->_m_pSharedMemory->data, data.c_str(), BUFFER_SIZE);
            }
            else {
                this->_m_pSharedMemory->data = data;
            }

            
            if (!ReleaseMutex(this->_m_hMutex)) {
                std::cerr << "Failed to release mutex (" << GetLastError() << ")." << std::endl;
            }
            
            if (!SetEvent(this->_m_hEventSend)) {
                std::cerr << "Failed to set event (" << GetLastError() << ")." << std::endl;
            }
            
            dwWaitResult = WaitForSingleObject(this->_m_hEventRead, INFINITE);
            if (dwWaitResult != WAIT_OBJECT_0) {
                std::cerr << "Failed to wait for read event (" << GetLastError() << ")." << std::endl;
            }
        }
        else {
            std::cerr << "Failed to acquire mutex (" << GetLastError() << ")." << std::endl;
        }
    }
};
