#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

class Client
{
private:
	HANDLE _m_pipe;

public:
	Client();

	int Connect(const std::string& serverName);
	int PushMessage(const std::string& message);
	void Disconnect();
}; 
