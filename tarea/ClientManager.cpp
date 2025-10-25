#include "ClientManager.h"

#include <iostream>

using namespace std;

void ClientManager::resolveClientMessages(int clientId)
{
    vector<unsigned char> buffer;
    bool logOut = false;

    do {
        recvMSG(clientId, buffer);
        msgTypes type = unpack<msgTypes>(buffer);

        switch (type) {
            case fileManagerConstructor: {
                string path;
                path.resize(unpack<long int>(buffer));
                unpackv(buffer, (char*)path.data(), path.size());
                FileManager newFileManager(path);
                fileManagerInstances[clientId] = newFileManager;
                buffer.clear();
            } break;
            case fileManagerDestructor: {
                fileManagerInstances.erase(clientId);
                buffer.clear();
                logOut = true;
            } break;
            case listFilesF: {
                vector<string> files = fileManagerInstances[clientId].listFiles();
                buffer.clear();
                long int numFiles = files.size();
                pack(buffer, numFiles);
                for (auto &file : files) {
                    long int len = file.size();
                    pack(buffer, len);
                    packv(buffer, (char*)file.data(), file.size());
                }
            } break;
            case readFileF: {
                string fileName;
                fileName.resize(unpack<long int>(buffer));
                unpackv(buffer, (char*)fileName.data(), fileName.size());
                vector<unsigned char> data;
                fileManagerInstances[clientId].readFile(fileName, data);
                buffer.clear();
                long int dataSize = data.size();
                pack(buffer, dataSize);
                packv(buffer, data.data(), data.size());
            } break;
            case writeFileF: {
                string fileName;
                fileName.resize(unpack<long int>(buffer));
                unpackv(buffer, (char*)fileName.data(), fileName.size());
                vector<unsigned char> data;
                long int dataSize = unpack<long int>(buffer);
                data.resize(dataSize);
                unpackv(buffer, data.data(), data.size());
                fileManagerInstances[clientId].writeFile(fileName, data);
                buffer.clear();
            } break;
            default:
                buffer.clear();
                break;
        }

        pack(buffer, ack);
        sendMSG(clientId, buffer);
    } while (!logOut);

    closeConnection(clientId);
}
