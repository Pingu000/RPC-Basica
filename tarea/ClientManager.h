#pragma once

#include "filemanager.h"
#include "utils.h"
#include <map>
#include <string>
#include <vector>

using namespace std;

class ClientManager {
public:
    enum msgTypes {
        fileManagerConstructor,
        fileManagerDestructor,
        listFilesF,
        readFileF,
        writeFileF,
        ack
    };

    static inline map<FileManager*, int> clientConnections;
    static inline map<int, FileManager> fileManagerInstances;

    static void resolveClientMessages(int clientId);
};
