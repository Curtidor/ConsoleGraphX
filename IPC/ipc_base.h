#include <windows.h>
#include <string>
#include <memory>
#include "shared_memory.h"

//TODO
// this class has a few bugs with timing and will be fixed in a later update
template <typename T>
class IPCBase {
protected:
    HANDLE _m_hMutex;
    HANDLE _m_hEvent;
    HANDLE _m_hMapFile;
    std::unique_ptr<SharedMemory<T>> _m_pSharedMemory;
    std::wstring _m_identifier;
    std::wstring _m_identifierMutex;
    std::wstring _m_identifierEvent;
    std::wstring _m_identifierMapFile;

public:
    IPCBase(const std::wstring& identifier = L"default") : _m_hMutex(nullptr), _m_hEvent(nullptr), _m_hMapFile(nullptr),
                                                           _m_identifier(identifier), _m_identifierEvent(identifier + L"Event"),
                                                           _m_identifierMutex(identifier + L"Mutex"), _m_identifierMapFile(identifier + L"MapFile") {}

    ~IPCBase() 
    {
        UnmapViewOfFile(this->_m_pSharedMemory.get());
        CloseHandle(this->_m_hMapFile);
        CloseHandle(this->_m_hEvent);
        CloseHandle(this->_m_hMutex);
    }
};
