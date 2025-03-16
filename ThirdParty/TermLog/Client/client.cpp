#include "client.h"
#include <iostream>

Client::Client(): _m_pipe(INVALID_HANDLE_VALUE)
{}

int Client::Connect(const std::string& serverName)
{
    std::string pipeName = "\\\\.\\pipe\\" + serverName;
    if (!WaitNamedPipeA(pipeName.c_str(), 10000)) // error 121
    {

        return GetLastError(); 
    }

    _m_pipe = CreateFileA(
        pipeName.c_str(),   // pipe name 
        GENERIC_WRITE,
        0,              // no sharing 
        NULL,           // default security attributes
        OPEN_EXISTING,  // opens existing pipe 
        0,              // default attributes 
        NULL);          // no template file 

    if (_m_pipe == INVALID_HANDLE_VALUE)
        return GetLastError();

    return 0;
}

int Client::PushMessage(const std::string& message)
{
    DWORD bytesWritten;
    bool writeStatus =  WriteFile(_m_pipe, message.c_str(), message.length(), &bytesWritten, NULL);

    if (!writeStatus)
        return GetLastError();
    
    return 0;
}

void Client::Disconnect()
{
    CloseHandle(_m_pipe);

    _m_pipe = INVALID_HANDLE_VALUE;
}

