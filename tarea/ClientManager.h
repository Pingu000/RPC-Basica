#pragma once

#include "filemanager.h"
#include "utils.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

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

    static inline std::map<FileManager*, int> clientConnections;
    static inline std::map<int, std::unique_ptr<FileManager>> fileManagerInstances;

    static void resolveClientMessages(int clientId);
};
