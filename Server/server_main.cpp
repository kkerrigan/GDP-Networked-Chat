#include "cTCPServer.h"
#include "cTCPAuthClient.h"

#include "../AuthenticationServer/AuthenticationProtocol.pb.h"
#include "../AuthenticationServer/cProtoBufHelper.h"

#include "../include/cConsole.h"
#include "../include/cBuffer.h"

#include <thread>
#include <atomic>
#include <iostream>
cTCPAuthClient authClient;
void AuthServerConnection(std::atomic<bool>& run)
{
	authClient.initializeWinsock();
	authClient.resolveServerAddr();
	if (authClient.connectToServer()) {
		std::cout << "Connected to the auth server." << std::endl;
	}

	authClient.startListen();
}

int main(int argc, char *argv[])
{

	std::atomic<bool> run(true);
	std::thread cinThread(AuthServerConnection, std::ref(run));

	cinThread.detach();


	cConsole console;
	int rowNum = 0;
	cTCPServer server;

	
	console.log("Initialising Winsock...");
	if (!server.initializeWinsock()) {
		console.log("Failed. Error Code : %d", WSAGetLastError());
	}

	console.log("Initialised.");

	//Create a socket
	if (!server.createMasterSocket())
	{
		console.log("Could not create socket : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	console.log("Socket created.");

	//Bind
	if (!server.bindSocket())
	{
		console.log("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	server.startListen();

	server.destroySocket();
	
	server.cleanUp();

	return 0;
}