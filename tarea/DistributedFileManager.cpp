#include "filemanager.h"
#include "ClientManager.h"
#include "utils.h"

#include <iostream>
#include <vector>

using namespace std;

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 1067

FileManager::FileManager()
{
        ready=false;
}

FileManager::FileManager(string path)
{
        ready=false;
        vector<unsigned char> buffer;
        connection_t connection=initClient(SERVER_IP,SERVER_PORT);
        int serverId=connection.serverId;

        pack(buffer,ClientManager::fileManagerConstructor);
        long int pathSize=path.size();
        pack(buffer,pathSize);
        packv(buffer,(char*)path.data(),path.size());
        sendMSG(serverId,buffer);

        buffer.clear();
        recvMSG(serverId,buffer);
        if(unpack<ClientManager::msgTypes>(buffer)!=ClientManager::ack)
                cout<<"ERROR: ACK not received\n";

        dirPath=path;
        ready=true;
        ClientManager::clientConnections[this]=serverId;
}

FileManager::~FileManager()
{
        vector<unsigned char> buffer;
        int serverId=ClientManager::clientConnections[this];
        pack(buffer,ClientManager::fileManagerDestructor);
        sendMSG(serverId,buffer);

        buffer.clear();
        recvMSG(serverId,buffer);
        if(unpack<ClientManager::msgTypes>(buffer)!=ClientManager::ack)
                cout<<"ERROR: ACK not received\n";

        closeConnection(serverId);
        ClientManager::clientConnections.erase(this);
        ready=false;
}

vector<string> FileManager::listFiles()
{
        vector<string> files;
        vector<unsigned char> buffer;
        int serverId=ClientManager::clientConnections[this];
        pack(buffer,ClientManager::listFilesF);
        sendMSG(serverId,buffer);

        buffer.clear();
        recvMSG(serverId,buffer);
        files.resize(unpack<long int>(buffer));
        for(auto &file:files){
                file.resize(unpack<long int>(buffer));
                unpackv(buffer,(char*)file.data(),file.size());
        }

        if(unpack<ClientManager::msgTypes>(buffer)!=ClientManager::ack)
                cout<<"ERROR: ACK not received\n";

        return files;
}

void FileManager::readFile(string fileName, vector<unsigned char> &data)
{
        vector<unsigned char> buffer;
        int serverId=ClientManager::clientConnections[this];
        pack(buffer,ClientManager::readFileF);
        long int nameSize=fileName.size();
        pack(buffer,nameSize);
        packv(buffer,(char*)fileName.data(),fileName.size());
        sendMSG(serverId,buffer);

        buffer.clear();
        recvMSG(serverId,buffer);
        data.resize(unpack<long int>(buffer));
        unpackv(buffer,data.data(),data.size());

        if(unpack<ClientManager::msgTypes>(buffer)!=ClientManager::ack)
                cout<<"ERROR: ACK not received\n";
}

void FileManager::writeFile(string fileName, vector<unsigned char> &data)
{
        vector<unsigned char> buffer;
        int serverId=ClientManager::clientConnections[this];
        pack(buffer,ClientManager::writeFileF);
        long int nameSize=fileName.size();
        pack(buffer,nameSize);
        packv(buffer,(char*)fileName.data(),fileName.size());
        long int dataSize=data.size();
        pack(buffer,dataSize);
        packv(buffer,data.data(),data.size());
        sendMSG(serverId,buffer);

        buffer.clear();
        recvMSG(serverId,buffer);
        if(unpack<ClientManager::msgTypes>(buffer)!=ClientManager::ack)
                cout<<"ERROR: ACK not received\n";
}
