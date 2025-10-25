#include "utils.h"
#include "ClientManager.h"

#include <iostream>
#include <string>
#include <thread>

int main(int argc, char** argv)
{
    bool exit = false;
    std::cout << "Server opening port\n";
    int serverPortId = initServer(1067);
    std::cout << "Server port opened, waiting for connections\n";
    while (!exit) {
        while (!checkClient())
            usleep(100);
        int clientId = getLastClientID();
        std::cout << "Client " << clientId << " connected\n";
        std::thread* th = new std::thread(ClientManager::resolveClientMessages, clientId);
        th->detach();
        delete th;
    }
    close(serverPortId);
    return 0;
}
