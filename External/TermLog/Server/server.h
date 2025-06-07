#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>

template <size_t BufferSize>
class Server
{
private:
    std::vector<HANDLE> _m_pipes;
    std::string _m_serverName;

public:
    Server(const std::string& serverName) : _m_serverName(serverName) {}

    bool Create()
    {
        std::string fullPipeName = "\\\\.\\pipe\\" + _m_serverName;

        HANDLE pipe = CreateNamedPipeA(
            fullPipeName.c_str(),
            PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED, // async mode for multiple clients
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES, // allow multiple clients
            0, // output buffer (not needed for read-only)
            BufferSize, // input buffer size
            0, // default timeout
            NULL // default security
        );

        if (pipe == INVALID_HANDLE_VALUE)
        {
            std::cerr << "Failed to create named pipe. Error: " << GetLastError() << std::endl;
            return false;
        }

        _m_pipes.push_back(pipe);
        return true;
    }

    bool HasActiveClient(HANDLE hPipe)
    {
        DWORD bytesAvailable = 0;
        BOOL success = PeekNamedPipe(hPipe, NULL, 0, NULL, &bytesAvailable, NULL);

        return success;
    }

    void HandleClient(HANDLE hPipe)
    {
        char buffer[BufferSize]{};
        DWORD bytesRead;

        while (true)
        {
            BOOL success = ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL);

            if (!success || bytesRead == 0)
            {
                DWORD error = GetLastError();
                if (error == ERROR_BROKEN_PIPE)
                {
                    std::cout << "Client disconnected.\n";
                }
                else
                {
                    std::cerr << "ReadFile error: " << error << std::endl;
                }
                break;
            }

            buffer[bytesRead] = '\0';
            std::cout << "[LOG] " << buffer << std::endl;
        }

        DisconnectNamedPipe(hPipe);
        CloseHandle(hPipe);
    }

    void WaitForClients()
    {
        std::vector<std::thread> clientThreads;

        while (true)
        {
            Create(); 
            HANDLE hPipe = _m_pipes.back();

            BOOL success = ConnectNamedPipe(hPipe, NULL);

            if (!success)
            {
                DWORD error = GetLastError();
                if (error == ERROR_PIPE_CONNECTED)
                {
                    std::cout << "A client already connected before ConnectNamedPipe was called." << std::endl;
                }
                else
                {
                    std::cerr << "Connection error: " << error << std::endl;
                    CloseHandle(hPipe);
                    continue;
                }
            }

            clientThreads.emplace_back(&Server::HandleClient, this, hPipe);
        }

        for (auto& t : clientThreads)
        {
            if (t.joinable())
            {
                t.join();
            }
        }
    }

    void Destroy()
    {
        for (HANDLE pipe : _m_pipes)
        {
            CloseHandle(pipe);
        }
        _m_pipes.clear();
    }
};