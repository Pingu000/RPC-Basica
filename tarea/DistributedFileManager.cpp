#include "filemanager.h"
#include "ClientManager.h"
#include "utils.h"

#include <iostream>
#include <vector>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 1067

FileManager::FileManager()
{
    ready = false;
}

FileManager::FileManager(std::string path)
{
    ready = false;
    std::vector<unsigned char> buffer;
    connection_t connection = initClient(SERVER_IP, SERVER_PORT);
    if (connection.socket < 0) {
        std::cout << "ERROR: Unable to connect to server" << std::endl;
        return;
    }

    int serverId = connection.serverId;

    pack(buffer, ClientManager::fileManagerConstructor);
    pack(buffer, static_cast<long int>(path.size()));
    if (!path.empty()) {
        packv(buffer, path.data(), path.size());
    }
    sendMSG(serverId, buffer);

    buffer.clear();
    recvMSG(serverId, buffer);
    if (unpack<ClientManager::msgTypes>(buffer) != ClientManager::ack) {
        std::cout << "ERROR: ACK not received" << std::endl;
        closeConnection(serverId);
        return;
    }

    dirPath = path;
    ready = true;
    ClientManager::clientConnections[this] = serverId;
}

FileManager::~FileManager()
{
    auto it = ClientManager::clientConnections.find(this);
    if (!ready || it == ClientManager::clientConnections.end()) {
        return;
    }

    int serverId = it->second;
    std::vector<unsigned char> buffer;
    pack(buffer, ClientManager::fileManagerDestructor);
    sendMSG(serverId, buffer);

    buffer.clear();
    recvMSG(serverId, buffer);
    if (unpack<ClientManager::msgTypes>(buffer) != ClientManager::ack) {
        std::cout << "ERROR: ACK not received" << std::endl;
    }

    closeConnection(serverId);
    ClientManager::clientConnections.erase(it);
    ready = false;
}

std::vector<std::string> FileManager::listFiles()
{
    std::vector<std::string> files;
    auto it = ClientManager::clientConnections.find(this);
    if (!ready || it == ClientManager::clientConnections.end()) {
        return files;
    }

    int serverId = it->second;
    std::vector<unsigned char> buffer;
    pack(buffer, ClientManager::listFilesF);
    sendMSG(serverId, buffer);

    buffer.clear();
    recvMSG(serverId, buffer);
    long int numFiles = unpack<long int>(buffer);
    if (numFiles < 0) {
        numFiles = 0;
    }
    files.resize(static_cast<size_t>(numFiles));
    for (auto &file : files) {
        long int len = unpack<long int>(buffer);
        if (len < 0) {
            len = 0;
        }
        file.resize(static_cast<size_t>(len));
        if (len > 0) {
            unpackv(buffer, file.data(), file.size());
        }
    }

    if (unpack<ClientManager::msgTypes>(buffer) != ClientManager::ack) {
        std::cout << "ERROR: ACK not received" << std::endl;
    }

    return files;
}

void FileManager::readFile(std::string fileName, std::vector<unsigned char> &data)
{
    data.clear();
    auto it = ClientManager::clientConnections.find(this);
    if (!ready || it == ClientManager::clientConnections.end()) {
        return;
    }

    int serverId = it->second;
    std::vector<unsigned char> buffer;
    pack(buffer, ClientManager::readFileF);
    pack(buffer, static_cast<long int>(fileName.size()));
    if (!fileName.empty()) {
        packv(buffer, fileName.data(), fileName.size());
    }
    sendMSG(serverId, buffer);

    buffer.clear();
    recvMSG(serverId, buffer);
    long int dataSize = unpack<long int>(buffer);
    if (dataSize < 0) {
        dataSize = 0;
    }
    data.resize(static_cast<size_t>(dataSize));
    if (dataSize > 0) {
        unpackv(buffer, data.data(), data.size());
    }

    if (unpack<ClientManager::msgTypes>(buffer) != ClientManager::ack) {
        std::cout << "ERROR: ACK not received" << std::endl;
    }
}

void FileManager::writeFile(std::string fileName, std::vector<unsigned char> &data)
{
    auto it = ClientManager::clientConnections.find(this);
    if (!ready || it == ClientManager::clientConnections.end()) {
        return;
    }

    int serverId = it->second;
    std::vector<unsigned char> buffer;
    pack(buffer, ClientManager::writeFileF);
    pack(buffer, static_cast<long int>(fileName.size()));
    if (!fileName.empty()) {
        packv(buffer, fileName.data(), fileName.size());
    }
    pack(buffer, static_cast<long int>(data.size()));
    if (!data.empty()) {
        packv(buffer, data.data(), data.size());
    }
    sendMSG(serverId, buffer);

    buffer.clear();
    recvMSG(serverId, buffer);
    if (unpack<ClientManager::msgTypes>(buffer) != ClientManager::ack) {
        std::cout << "ERROR: ACK not received" << std::endl;
    }
}
