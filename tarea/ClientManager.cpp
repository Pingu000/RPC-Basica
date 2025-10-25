#include "ClientManager.h"

#include <iostream>
#include <memory>

void ClientManager::resolveClientMessages(int clientId)
{
    std::vector<unsigned char> buffer;
    bool logOut = false;

    do {
        recvMSG(clientId, buffer);
        msgTypes type = unpack<msgTypes>(buffer);

        switch (type) {
            case fileManagerConstructor: {
                std::string path;
                path.resize(unpack<long int>(buffer));
                if (!path.empty()) {
                    unpackv(buffer, path.data(), path.size());
                }
                fileManagerInstances[clientId] = std::make_unique<FileManager>(path);
                buffer.clear();
            } break;
            case fileManagerDestructor: {
                auto it = fileManagerInstances.find(clientId);
                if (it != fileManagerInstances.end()) {
                    fileManagerInstances.erase(it);
                }
                buffer.clear();
                logOut = true;
            } break;
            case listFilesF: {
                auto it = fileManagerInstances.find(clientId);
                std::vector<std::string> files;
                if (it != fileManagerInstances.end()) {
                    files = it->second->listFiles();
                }
                buffer.clear();
                pack(buffer, static_cast<long int>(files.size()));
                for (auto &file : files) {
                    pack(buffer, static_cast<long int>(file.size()));
                    if (!file.empty()) {
                        packv(buffer, file.data(), file.size());
                    }
                }
            } break;
            case readFileF: {
                std::string fileName;
                fileName.resize(unpack<long int>(buffer));
                if (!fileName.empty()) {
                    unpackv(buffer, fileName.data(), fileName.size());
                }
                std::vector<unsigned char> data;
                auto it = fileManagerInstances.find(clientId);
                if (it != fileManagerInstances.end()) {
                    it->second->readFile(fileName, data);
                }
                buffer.clear();
                pack(buffer, static_cast<long int>(data.size()));
                if (!data.empty()) {
                    packv(buffer, data.data(), data.size());
                }
            } break;
            case writeFileF: {
                std::string fileName;
                fileName.resize(unpack<long int>(buffer));
                if (!fileName.empty()) {
                    unpackv(buffer, fileName.data(), fileName.size());
                }
                std::vector<unsigned char> data;
                long int dataSize = unpack<long int>(buffer);
                if (dataSize > 0) {
                    data.resize(dataSize);
                    unpackv(buffer, data.data(), data.size());
                }
                auto it = fileManagerInstances.find(clientId);
                if (it != fileManagerInstances.end()) {
                    it->second->writeFile(fileName, data);
                }
                buffer.clear();
            } break;
            default:
                std::cerr << "ERROR: Unknown message type received" << std::endl;
                buffer.clear();
                break;
        }

        pack(buffer, ack);
        sendMSG(clientId, buffer);
    } while (!logOut);

    closeConnection(clientId);
}
