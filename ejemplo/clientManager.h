#pragma once

#include "database.h"
#include "utils.h"
#include <string>

using namespace std;

class clientManager{
public:

	typedef enum{
		databaseConstructor,
		databaseDestructor,
		addRecordF,
		addRecordSetF,
		getRecordF,
		addComplexRecordF,
		ack
	}msgTypes;


	static inline map<database*,int> clientConnections;
	static inline map<int,database> databaseInstances;// instancias de clase database creadas por usuarios. Pares Clave=ClientId, valor=Instancia database
	
	static void resolveClientMessages(int clientId);//atiendeCliente
	
};