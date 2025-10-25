#include "clientManager.h"

void clientManager::resolveClientMessages(int clientId){

	string userName="";
	vector<unsigned char> buffer;
	bool logOut=false;
	do{
	//receive a packet from client
		recvMSG(clientId,buffer);
		msgTypes type=unpack<msgTypes>(buffer);
	//switch type of the packet
		switch(type){
		//type login
			case databaseConstructor:
			{
				//crear base 
				string param1;
				//desempaquetar parametro
				param1.resize(unpack<long int>(buffer));
				unpackv(buffer, (char*)param1.data(),param1.size());
				database newDB(param1);
				//almacenamos
				databaseInstances[clientId]=newDB;
				buffer.clear();
			}break;
			case databaseDestructor:
			{
				//borrar base datos
				databaseInstances.erase(clientId);
				buffer.clear();
				logOut=true;
			}break;
			case addRecordF:
			{
				//desempaquetar parametros key data
				
				string p1;
				string p2;
				p1.resize(unpack<long int>(buffer));
				unpackv(buffer,(char*)p1.data(),p1.size());
				p2.resize(unpack<long int>(buffer));
				unpackv(buffer,(char*)p2.data(),p2.size());
				//invocamos funcion
				bool res=databaseInstances[clientId].addRecord(p1,p2);
				//empaquetar resultado
				buffer.clear();
				pack(buffer,res);
				
			}break;
			case addRecordSetF:{
				//desemaquetar p1
				string p1;
				vector<string> p2;
				p1.resize(unpack<long int>(buffer));
				unpackv(buffer,(char*)p1.data(),p1.size());
				//redimensionar vector
				p2.resize(unpack<long int>(buffer));
				//desempaquetar cada elemento de vector

				for(auto &str: p2)
				{
					str.resize(unpack<long int>(buffer));
					unpackv(buffer,(char*)str.data(),str.size());
				}
				//invocamos funcion
				bool res=databaseInstances[clientId].addRecordSet(p1,p2);
				//empaquetar resultado
				buffer.clear();
				pack(buffer,res);
			
			}break;
			
			case getRecordF:{
				//desempaquetar P1 P2
				string p1;
				int p2;
				p1.resize(unpack<long int>(buffer));
				unpackv(buffer,(char*)p1.data(),p1.size());
				p2=unpack<int>(buffer);
				
				//ejecutar
				string res=databaseInstances[clientId].getRecord(p1,p2);
				//empaquetar resultado
				buffer.clear();
				pack(buffer, res.size());
				packv(buffer, res.data(), res.size());

			}break;
			case addComplexRecordF:
			{
				//recibir parametros
				string p1;
				datoComplejo_t p2;
				
				p1.resize(unpack<long int>(buffer));
				unpackv(buffer,(char*)p1.data(),p1.size());
				p2=unpack<datoComplejo_t>(buffer);

				//invocar 
				databaseInstances[clientId].addComplexRecord(p1,p2);
				//devolver resultado
				buffer.clear();
			}break;
			
			default:
			//gestión de errores
			break;
		}
		//
		pack(buffer,ack);
		sendMSG(clientId,buffer);
	}while(!logOut);
	//close connection

	closeConnection(clientId);

}
