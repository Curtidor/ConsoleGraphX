#pragma once
#include <iostream>
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
class Receiver : public IPCBase<T> {
private:
    std::function<void(T)> _m_onReceive;
    bool _m_isRunning = false;

public:
    Receiver(const std::wstring& identifier, std::function<void(T)> onReceive)
        : IPCBase<T>(identifier), _m_onReceive(onReceive)
    {
        this->_m_hMutex = OpenMutex(SYNCHRONIZE, FALSE, this->_m_identifierMutex.c_str());
        if (this->_m_hMutex == NULL) {
            throw std::runtime_error("OpenMutex error: " + std::to_string(GetLastError()));
        }

        this->_m_hEventSend = OpenEvent(SYNCHRONIZE, FALSE, (this->_m_identifierEvent + L"Send").c_str());
        if (this->_m_hEventSend == NULL) {
            CloseHandle(this->_m_hMutex);
            throw std::runtime_error("OpenEvent error: " + std::to_string(GetLastError()));
        }

        // added EVENT_MODIFY_STATE since the event is created in the sender process
        this->_m_hEventRead = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, (this->_m_identifierEvent + L"Read").c_str());
        if (this->_m_hEventRead == NULL) {
            CloseHandle(this->_m_hEventSend);
            CloseHandle(this->_m_hMutex);
            throw std::runtime_error("OpenEvent error: " + std::to_string(GetLastError()));
        }

        this->_m_hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, this->_m_identifierMapFile.c_str());
        if (this->_m_hMapFile == NULL) {
            CloseHandle(this->_m_hEventRead);
            CloseHandle(this->_m_hEventSend);
            CloseHandle(this->_m_hMutex);
            throw std::runtime_error("OpenFileMapping error: " + std::to_string(GetLastError()));
        }

        auto ptr = static_cast<SharedMemory<T>*>(MapViewOfFile(this->_m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory<T>)));
        if (ptr == nullptr) {
            CloseHandle(this->_m_hMapFile);
            CloseHandle(this->_m_hEventRead);
            CloseHandle(this->_m_hEventSend);
            CloseHandle(this->_m_hMutex);
            throw std::runtime_error("MapViewOfFile error: " + std::to_string(GetLastError()));
        }
        this->_m_pSharedMemory.reset(ptr);
    }

    void StartReceiver() {
        if (this->_m_isRunning)
            return;

        this->_m_isRunning = true;

        while (this->_m_isRunning) {
            DWORD dwWaitResult = WaitForSingleObject(this->_m_hEventSend, INFINITE);
            if (dwWaitResult == WAIT_OBJECT_0) {
                WaitForSingleObject(this->_m_hMutex, INFINITE);

                T data = this->_m_pSharedMemory->data;

                ReleaseMutex(this->_m_hMutex);

                _m_onReceive(data);

                if (!SetEvent(this->_m_hEventRead)) {
                    std::cerr << "Failed to set read event (" << GetLastError() << ")." << std::endl;
                }
            }
            else {
                std::cerr << "WaitForSingleObject error: " << GetLastError() << std::endl;
                break;
            }
        }
    }

    void End() {
        _m_isRunning = false;
    }
};
