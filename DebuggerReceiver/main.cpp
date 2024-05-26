#include <string>
#include <iostream>
#include "../IPC/receiver.h"

int main()
{
	auto callback = [](std::string message) {std::cout << message << &message << std::endl; };
	
	Receiver<std::string> receiver(L"Debugger", callback);

	receiver.Start();
}