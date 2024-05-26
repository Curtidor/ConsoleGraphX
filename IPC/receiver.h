#pragma once
#include <windows.h>
#include <chrono>
#include <memoryapi.h>
#include <synchapi.h>
#include <functional>
#include <stdexcept>
#include <string>
#include <memory>
#include <type_traits>
#include "shared_memory.h"
#include "ipc_base.h"

template <typename T>
class Receiver : public IPCBase<T>
{
private:
    std::function<void(T)> _m_onReceive;
    bool _m_isRunning = false;

public:
    Receiver(const std::wstring& identifier, std::function<void(T)> onReceive) : IPCBase<T>(identifier), _m_onReceive(onReceive)
    {
        this->_m_hMutex = OpenMutex(SYNCHRONIZE, FALSE, this->_m_identifierMutex.c_str());
        if (this->_m_hMutex == NULL) {
            throw std::runtime_error("OpenMutex error: " + GetLastError());
        }

        this->_m_hEventSend = OpenEvent(SYNCHRONIZE, FALSE, (this->_m_identifierEvent + L"Send").c_str());
        if (this->_m_hEventSend == NULL) {
            CloseHandle(this->_m_hMutex);
            throw std::runtime_error("OpenEvent error: " + GetLastError());
        }

        this->_m_hEventRead = OpenEvent(SYNCHRONIZE, FALSE, (this->_m_identifierEvent + L"Read").c_str());
        if (this->_m_hEventRead == NULL) {
            CloseHandle(this->_m_hMutex);
            CloseHandle(this->_m_hEventSend);
            throw std::runtime_error("OpenEvent error: " + GetLastError());
        }

        this->_m_hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, this->_m_identifierMapFile.c_str());
        if (this->_m_hMapFile == NULL) {
            CloseHandle(this->_m_hEventRead);
            CloseHandle(this->_m_hEventSend);
            CloseHandle(this->_m_hMutex);
            throw std::runtime_error("OpenFileMapping error: " + GetLastError());
        }

        this->_m_pSharedMemory.reset((SharedMemory<T>*)MapViewOfFile(this->_m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory<T>)));
        if (this->_m_pSharedMemory == nullptr) {
            CloseHandle(this->_m_hMapFile);
            CloseHandle(this->_m_hEventSend);
            CloseHandle(this->_m_hEventRead);
            CloseHandle(this->_m_hMutex);
            throw std::runtime_error("MapViewOfFile error: " + GetLastError());
        }
    }

    void Start() {
        if (this->_m_isRunning)
            return;

        this->_m_isRunning = true;

        // Ensure the receiver initially signals that it's ready to read
        SetEvent(this->_m_hEventRead);

        while (this->_m_isRunning) {
            // Wait for the sender to signal that new data is available
            WaitForSingleObject(this->_m_hEventSend, INFINITE);

            // Acquire the mutex to access the shared memory
            WaitForSingleObject(this->_m_hMutex, INFINITE);

            std::cout << "GOT SEND " << std::chrono::high_resolution_clock::now().time_since_epoch().count() << std::endl;


            // Read the data from shared memory
            T data;
            if constexpr (std::is_same<T, std::string>::value) {
                data = std::string(this->_m_pSharedMemory->data);
            }
            else {
                data = this->_m_pSharedMemory->data;
            }

            // Process the received data
            this->_m_onReceive(data);

            // Release the mutex after reading the data
            ReleaseMutex(this->_m_hMutex);

            // Signal the sender that the data has been read
            std::cout << "SET READ B " << std::chrono::high_resolution_clock::now().time_since_epoch().count() << std::endl;
            SetEvent(this->_m_hEventRead);
            std::cout << "SET READ A " << std::chrono::high_resolution_clock::now().time_since_epoch().count() << std::endl;
        }

        // Perform any cleanup operations
        this->End();
    }



    void End()
    {
        _m_isRunning = false;
    }
};