#pragma once
#include <windows.h>
#include <string>

class Client {
public:
    Client() : _m_pipe(INVALID_HANDLE_VALUE) {}

    /**
     * @brief Connects to a named pipe server.
     *
     * @param serverName The name of the server (pipe name).
     * @return int 0 on success, or Windows error code on failure.
     */
    int Connect(const std::string& serverName)
    {
        std::string pipeName = "\\\\.\\pipe\\" + serverName;
        if (!WaitNamedPipeA(pipeName.c_str(), 10000)) // waits 10 seconds
            return GetLastError();

        _m_pipe = CreateFileA(
            pipeName.c_str(),   // pipe name 
            GENERIC_WRITE,
            0,                  // no sharing 
            NULL,               // default security
            OPEN_EXISTING,      // open existing
            0,                  // default flags
            NULL);              // no template

        if (_m_pipe == INVALID_HANDLE_VALUE)
            return GetLastError();

        return 0;
    }

    /**
     * @brief Sends a message to the server over the pipe.
     *
     * @param message The message string to send.
     * @return int 0 on success, or Windows error code on failure.
     */
    int PushMessage(const std::string& message)
    {
        if (_m_pipe == INVALID_HANDLE_VALUE)
            return ERROR_INVALID_HANDLE;

        DWORD bytesWritten;
        BOOL success = WriteFile(
            _m_pipe,
            message.c_str(),
            static_cast<DWORD>(message.size()),
            &bytesWritten,
            NULL);

        return success ? 0 : GetLastError();
    }

    /**
     * @brief Disconnects from the pipe server.
     */
    void Disconnect()
    {
        if (_m_pipe != INVALID_HANDLE_VALUE)
        {
            CloseHandle(_m_pipe);
            _m_pipe = INVALID_HANDLE_VALUE;
        }
    }

    /**
     * @brief Destructor ensures pipe is closed.
     */
    ~Client()
    {
        Disconnect();
    }

private:
    HANDLE _m_pipe;
};
