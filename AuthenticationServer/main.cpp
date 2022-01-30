#include <iostream>

#include "cDatabaseHelper.h"
#include "cHashGenerator.h"
#include "cTCPAuthServer.h"

cAuthTCPServer tcpAuthServer;

int main() {

	//start the server
	if (!tcpAuthServer.initializeWinsock()) {
		std::cout << "Failed. Error Code : " << WSAGetLastError();
	}

	//Create a socket
	if (!tcpAuthServer.createMasterSocket())
	{
		std::cout << "Failed. Error Code : " << WSAGetLastError();
		exit(EXIT_FAILURE);
	}

	//Bind
	if (!tcpAuthServer.bindSocket())
	{
		std::cout << "Failed. Error Code : " << WSAGetLastError();
		exit(EXIT_FAILURE);
	}

	tcpAuthServer.startListen();

	tcpAuthServer.destroySocket();

	tcpAuthServer.cleanUp();

	return 0;
}