#include <memory>

#include "cTCPServer.h"
#include "../include/cMessageProtocolHelper.h"
#include "cServerMessageHandler.h"
#include "cTCPServer.h"
#include "../include/cMessageProtocolHelper.h"
#include "cServerMessageHandler.h"

// c'tor
cTCPServer::cTCPServer() {
	this->buffer = (char*)malloc(this->MAX_RECV * sizeof(char));

	for (int i = 0; i < this->MAX_CLIENTS; i++)
	{
		this->client_sockets[i] = 0;
	}
}

// d'tor
cTCPServer::~cTCPServer() {
	delete this->buffer;
}

/*
*	initializeWinsock()
*	This function does the winsock setup for the server.
*	Accepts: None
*	Returns: A bool representing whether setup was successful.
*/
bool cTCPServer::initializeWinsock() {
	return (WSAStartup(MAKEWORD(2, 2), &this->winsockData) == 0) ? true : false;
}

/*
*	createMasterSocket()
*	This function sets the master socket for the server.
*	Accepts: None
*	Returns: A bool representing whether creating master socket was successful.
*/
bool cTCPServer::createMasterSocket() {
	if ((this->masterSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		return false;
	}

	//Prepare the sockaddr_in structure
	this->serverAddressInfo.sin_family = AF_INET;
	this->serverAddressInfo.sin_addr.s_addr = INADDR_ANY;
	this->serverAddressInfo.sin_port = htons(8888);

	return true;
}

/*
*	bindSocket()
*	This function binds the master socket to the server.
*	Accepts: None
*	Returns: A bool representing whether bind was successful.
*/
bool cTCPServer::bindSocket() {
	return (bind(this->masterSocket, (struct sockaddr *)&this->serverAddressInfo, sizeof(this->serverAddressInfo))) != SOCKET_ERROR ? true : false;
}

/*
*	startListen()
*	This function listens for recieved messages from the connected clients until 
	the client closes the connection.
*	Accepts: None
*	Returns: None
*/
void cTCPServer::startListen() {
	listen(this->masterSocket, 3);

	this->addressLength = sizeof(struct sockaddr_in);

	for(;;)
	{
		//clear the socket fd set
		FD_ZERO(&this->readfds);

		//add master socket to fd set
		FD_SET(this->masterSocket, &this->readfds);

		//add child sockets to fd set
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			this->currentSocket = this->client_sockets[i];
			if (this->currentSocket > 0)
			{
				FD_SET(this->currentSocket, &this->readfds);
			}
		}

		//wait for an activity on any of the sockets, timeout is NULL , so wait indefinitely
		this->activity = select(0, &this->readfds, NULL, NULL, NULL);

		if (this->activity == SOCKET_ERROR)
		{
			printf("select call failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		//If something happened on the master socket , then its an incoming connection
		if (FD_ISSET(this->masterSocket, &this->readfds))
		{
			if ((this->new_socket = accept(this->masterSocket, (struct sockaddr *)&this->clientAddress, (int *)&this->addressLength)) < 0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}

			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d\n", this->new_socket);

			//add new socket to array of sockets
			for (int i = 0; i < this->MAX_CLIENTS; i++)
			{
				if (this->client_sockets[i] == 0)
				{
					this->client_sockets[i] = this->new_socket;
					printf("Adding to list of sockets at index %d \n", i);
					break;
				}
			}
		}

		//else its some IO operation on some other socket :)
		for (int i = 0; i < this->MAX_CLIENTS; i++)
		{
			this->currentSocket = this->client_sockets[i];
			//if client presend in read sockets             
			if (FD_ISSET(this->currentSocket, &this->readfds))
			{
				//get details of the client
				getpeername(this->currentSocket, (struct sockaddr*)&this->clientAddress, (int*)&this->addressLength);

				// clear the server buffer
				for (int i = 0; i < this->MAX_RECV; ++i) {
					this->buffer[0] = '\0';
				}

				//Check if it was for closing , and also read the incoming message
				//recv does not place a null terminator at the end of the string (whilst printf %s assumes there is one).
				this->valread = recv(this->currentSocket, this->buffer, this->MAX_RECV, 0);

				if (this->valread == SOCKET_ERROR)
				{
					int error_code = WSAGetLastError();
					if (error_code == WSAECONNRESET)
					{
						//Somebody disconnected , get his details and print
						printf("Host disconnected unexpectedly\n");

						//Close the socket and mark as 0 in list for reuse
						closesocket(this->currentSocket);
						this->client_sockets[i] = 0;
					}
					else
					{
						printf("recv failed with error code : %d", error_code);
					}
				} else if (this->valread == 0)
				{
					//Somebody disconnected , get his details and print
					printf("Host disconnected\n");

					cServerMessageHandler::getInstance()->removeUserOnDisconnect(this->currentSocket);

					//Close the socket and mark as 0 in list for reuse
					closesocket(this->currentSocket);
					this->client_sockets[i] = 0;
				}
				else
				{
					cBuffer tempBuffer;
					
					//send only this packet to the handles and save the rest for later
					tempBuffer.loadBuffer(this->buffer, this->valread);
					
					this->recvBuffer[i].appendToBuffer(tempBuffer);

					//send the message now that we know the type to the message handler
					cServerMessageHandler* messageHandler = cServerMessageHandler::getInstance();
					messageHandler->handleMessage(this->recvBuffer[i], this->client_sockets[i]);
				}
			}
		}
	}
}

/*
*	destroySocket()
*	This function closes the current client's socket connection.
*	Accepts: None
*	Returns: None
*/
void cTCPServer::destroySocket() {
	closesocket(this->currentSocket);
}

/*
*	cleanUp()
*	This function cleans up winsock once server connection is closed.
*	Accepts: None
*	Returns: None
*/
void cTCPServer::cleanUp() {
	WSACleanup();
}