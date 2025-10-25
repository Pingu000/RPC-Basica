#include "utils.h"
#include "ClientManager.h"

#include <iostream>
#include <string>
#include <thread>

using namespace std;

int main(int argc, char** argv)
{
    bool exit = false;
    cout << "Server opening port\n";
    int serverPortId = initServer(1067);
    cout << "Server port opened, waiting for connections\n";
    while (!exit) {
        while (!checkClient())
            usleep(100);
        int clientId = getLastClientID();
        cout << "Client " << clientId << " connected\n";
        thread* th = new thread(ClientManager::resolveClientMessages, clientId);
        th->detach();
        delete th;
    }
    close(serverPortId);
    return 0;
}
