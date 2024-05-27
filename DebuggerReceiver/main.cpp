#include <string>
#include <iostream>
#include "../IPC/receiver.h"

int main()
{
	auto callback = [](std::string message) {std::cout << message << std::endl; };
	
	Receiver<std::string> receiver(L"Debugger", callback);

	receiver.StartReceiver();
}