#pragma once
#include <windows.h>
#include <memoryapi.h>
#include <synchapi.h>
#include <functional>
#include <queue>
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
    T _m_shutDownValue;
    std::queue<T> _m_queue;
    bool _m_isRunning = false;
    std::function<void(T)> _m_onReceive;

public:
    Receiver(const std::wstring& identifier, std::function<void(T)> onReceive, T shutDownValue) : IPCBase<T>(identifier), _m_onReceive(onReceive), _m_shutDownValue(shutDownValue)
    {
        this->_m_hMutex = OpenMutex(SYNCHRONIZE, FALSE, this->_m_identifierMutex.c_str());
        if (this->_m_hMutex == NULL) {
            throw std::runtime_error("OpenMutex error: " + GetLastError());
        }

        this->_m_hEvent = OpenEvent(SYNCHRONIZE, FALSE, this->_m_identifierEvent.c_str());
        if (this->_m_hEvent == NULL) {
            throw std::runtime_error("OpenEvent error: " + GetLastError());
        }

        this->_m_hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, this->_m_identifierMapFile.c_str());
        if (this->_m_hMapFile == NULL) {
            throw std::runtime_error("OpenFileMapping error: " + GetLastError());
        }

        this->_m_pSharedMemory.reset((SharedMemory<T>*)MapViewOfFile(this->_m_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory<T>)));
        if (this->_m_pSharedMemory == nullptr) {
            CloseHandle(this->_m_hMapFile);
            CloseHandle(this->_m_hEvent);
            CloseHandle(this->_m_hMutex);
            throw std::runtime_error("MapViewOfFile error: " + GetLastError());
        }
    }

    void Start() {
        if (this->_m_isRunning)
            return;

        this->_m_isRunning = true;

        while (this->_m_isRunning) {
            // Wait for the event to be signaled
            WaitForSingleObject(this->_m_hEvent, INFINITE);

            // Acquire the mutex to access the shared memory
            WaitForSingleObject(this->_m_hMutex, INFINITE);

            // Retrieve the data from shared memory
            data = this->_m_pSharedMemory->data;
            
            _m_queue.push(data);

            // Release the mutex after retrieving the data
            ReleaseMutex(this->_m_hMutex);

            // Process the data
            while (!_m_queue.empty()) {
                // Retrieve the next message from the queue
                T queueItem = _m_queue.front();

                _m_queue.pop();

                // Process the message
                if constexpr (std::is_same<T, std::string>::value) {
                    this->_m_onReceive(std::string(queueItem));
                }
                else {
                    this->_m_onReceive(queueItem);
                }

                // Check for shutdown signal
                if (queueItem == _m_shutDownValue) {
                    _m_isRunning = false;
                    break; // Exit the loop immediately upon receiving the shutdown signal
                }
            }
        }

        // Perform any cleanup operations
        this->End();
    }



    void End()
    {
        _m_isRunning = false;
    }
};