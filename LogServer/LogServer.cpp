#include <iostream>
#include "../External/TermLog/Server/server.h"

int main()
{
    Server<512> server = Server<512>("EditorLogger");
    server.Create();

    server.WaitForClients();
}

