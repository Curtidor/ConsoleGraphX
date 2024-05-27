#include <windows.h>
#include <string>
#include <memory>
#include <stdexcept>
#include "shared_memory.h"

template <typename T>
class IPCBase {
protected:
    HANDLE _m_hMutex;
    HANDLE _m_hMapFile;
    HANDLE _m_hEventSend;
    HANDLE _m_hEventRead;

    std::wstring _m_identifierMutex;
    std::wstring _m_identifierEvent;
    std::wstring _m_identifierMapFile;

    std::unique_ptr<SharedMemory<T>, void(*)(SharedMemory<T>*)> _m_pSharedMemory;

public:
    IPCBase(const std::wstring& identifier = L"default")
        : _m_hMutex(nullptr), _m_hMapFile(nullptr), _m_hEventSend(nullptr), _m_hEventRead(nullptr),
        _m_pSharedMemory(nullptr, [](SharedMemory<T>* ptr) { if (ptr) UnmapViewOfFile(ptr); }),
        _m_identifierEvent(identifier + L"Event"), _m_identifierMutex(identifier + L"Mutex"), _m_identifierMapFile(identifier + L"MapFile")
    {}

    ~IPCBase()
    {
        // custom deleter in unique_ptr will handle UnmapViewOfFile
        CloseHandle(this->_m_hMapFile);
        CloseHandle(this->_m_hEventSend);
        CloseHandle(this->_m_hEventRead);
        CloseHandle(this->_m_hMutex);
    }
};
