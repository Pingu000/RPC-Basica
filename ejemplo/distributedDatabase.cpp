#include "database.h"
#include "utils.h"
#include "clientManager.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 1067

database::database(string name){
	vector<unsigned char> buffer;
	//iniciar conexión
	
	int serverId= initClient(SERVER_IP,SERVER_PORT).serverId;
	
	//enviar mensaje tipo constructor
		//pack tipo constructor
	pack(buffer,clientManager::databaseConstructor);
		//pack parametros
	pack(buffer, name.size());
	packv(buffer, name.data(), name.size());
	
	sendMSG(serverId,buffer);
	//recibir ack
	buffer.clear();
	recvMSG(serverId,buffer);
	if(unpack<clientManager::msgTypes> (buffer) !=clientManager::ack)
		cout<<"ERROR: ACK no recibido\n";
	
	//almacenar serverId
	clientManager::clientConnections[this]=serverId;
	cout<<"Base de datos conectada\n";

}

bool database::addRecord(string key,string data){
	vector<unsigned char> buffer;
	//iniciar conexión
	int serverId=clientManager::clientConnections[this];

	//pack tipo mensaje	
	pack(buffer,clientManager::addRecordF);
	//
	//pack parametros
	pack(buffer, key.size());
	packv(buffer, key.data(), key.size());
	pack(buffer, data.size());
	packv(buffer, data.data(), data.size());

	sendMSG(serverId,buffer);
	
	//recibir resultados
	buffer.clear();
	recvMSG(serverId,buffer);
	//unpack resultado
	bool resultado=unpack<bool>(buffer);
	//unpack ack
	if(unpack<clientManager::msgTypes> (buffer) !=clientManager::ack)
		cout<<"ERROR: ACK no recibido\n";
	
	return resultado;
}

bool database::addRecordSet(string key,vector<string> dataSet){
	
	vector<unsigned char> buffer;
	int serverId=clientManager::clientConnections[this];
	//crear mensaje addRecordSet
	pack(buffer,clientManager::addRecordSetF);
	
	//pack parametro 1
	pack(buffer, key.size());
	packv(buffer, key.data(), key.size());
	//pack parametro 2
		//pack num string
	pack(buffer, dataSet.size());
		//por cada uno
	for(auto &str: dataSet){
			//pack size
			//pack data
		pack(buffer, str.size());
		packv(buffer, str.data(), str.size());
	}
		
	sendMSG(serverId,buffer);
	
	//recibir resultados
	buffer.clear();
	recvMSG(serverId,buffer);
	//unpack resultado
	bool resultado=unpack<bool>(buffer);
	//unpack ack
	if(unpack<clientManager::msgTypes> (buffer) !=clientManager::ack)
		cout<<"ERROR: ACK no recibido\n";
	
	return resultado;

}
string database::getRecord(string key,int position)
{
	
	vector<unsigned char> buffer;
	int serverId=clientManager::clientConnections[this];
	//crear mensaje getRecord
	pack(buffer,clientManager::getRecordF);
//pack parametro 1
	pack(buffer, key.size());
	packv(buffer, key.data(), key.size());
	pack(buffer, position);

	sendMSG(serverId,buffer);
	
	//recibir resultados
	buffer.clear();
	recvMSG(serverId,buffer);
	//unpack resultado
	string resultado;
	resultado.resize(unpack<long int>(buffer));
	unpackv(buffer,(char*)resultado.data(), resultado.size());
	//unpack ack
	if(unpack<clientManager::msgTypes> (buffer) !=clientManager::ack)
		cout<<"ERROR: ACK no recibido\n";

	return resultado;
}

database::~database(){
	vector<unsigned char> buffer;
	int serverId=clientManager::clientConnections[this];
	//crear mensaje destructor
	pack(buffer,clientManager::databaseDestructor);

	//enviar 
	sendMSG(serverId,buffer);
	//recibir ack
	buffer.clear();
	recvMSG(serverId,buffer);
	if(unpack<clientManager::msgTypes> (buffer) !=clientManager::ack)
		cout<<"ERROR: ACK no recibido\n";

//cerrar conexión
	closeConnection(serverId);
	cout<<"Base de datos desconectada\n";
	
//borrar serverId de mapa
	clientManager::clientConnections.erase(this);
	
}



void database::addComplexRecord(string key,datoComplejo_t data)
{
	
	vector<unsigned char> buffer;
	int serverId=clientManager::clientConnections[this];
	//crear mensaje complex
	pack(buffer,clientManager::addComplexRecordF);

//pack parametro 1
	pack(buffer, key.size());
	packv(buffer, key.data(), key.size());
//pack param2
	pack(buffer, data);
	//enviamos
	sendMSG(serverId,buffer);
	
	//recibir ack
	buffer.clear();
	recvMSG(serverId,buffer);
	if(unpack<clientManager::msgTypes> (buffer) !=clientManager::ack)
		cout<<"ERROR: ACK no recibido\n";

}
