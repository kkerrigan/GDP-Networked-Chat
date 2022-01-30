#include "cTCPAuthClient.h"
#include "cAuthenticationClientMessageHandler.h"

// c'tor
cTCPAuthClient::cTCPAuthClient()
{
	this->connectSocket = INVALID_SOCKET;
	this->infoResult = NULL;
	this->connectAttempt = NULL;
}

// d'tor
cTCPAuthClient::~cTCPAuthClient()
{
}

/*
*	initializeWinsock()
*	This function does the winsock setup for the client.
*	Accepts: None
*	Returns: A bool representing whether setup was successful.
*/
bool cTCPAuthClient::initializeWinsock() {
	bool result = WSAStartup(MAKEWORD(2, 2), &this->winsockData) == 0 ? true : false;

	if (!result) {
		return false;
	}

	ZeroMemory(&this->hints, sizeof(this->hints));
	this->hints.ai_family = AF_UNSPEC;
	this->hints.ai_socktype = SOCK_STREAM;
	this->hints.ai_protocol = IPPROTO_TCP;

	return true;
}

/*
*	resolveServerAddr()
*	This function determines the address information on the server.
*	Accepts: None
*	Returns: A bool representing whether the winsock call was successful.
*/
bool cTCPAuthClient::resolveServerAddr() {
	return getaddrinfo(SERVER, DEFAULT_PORT, &this->hints, &this->infoResult) == 0 ? true : false;
}

/*
*	connectToServer()
*	This function connects the client to the server.
*	Accepts: None
*	Returns: A bool representing whether connecting to the server was successful.
*/
bool cTCPAuthClient::connectToServer() {
	// Step #3 Attempt to connect to an address until one succeeds
	for (this->connectAttempt = infoResult; this->connectAttempt != NULL; this->connectAttempt = this->connectAttempt->ai_next)
	{
		// Create a SOCKET for connecting to server
		this->connectSocket = socket(this->connectAttempt->ai_family, this->connectAttempt->ai_socktype, this->connectAttempt->ai_protocol);

		if (this->connectSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return false;
		}

		// Connect to server.
		int result = connect(this->connectSocket, this->connectAttempt->ai_addr, (int)this->connectAttempt->ai_addrlen);
		if (result == SOCKET_ERROR)
		{
			closesocket(this->connectSocket);
			this->connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(this->infoResult);

	if (this->connectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return false;
	}

	return true;
}

/*
*	startListen()
*	This function listens for recieved messages from the server until the connection is closed.
*	Accepts: None
*	Returns: None
*/
void cTCPAuthClient::startListen() {

	int result = 0;
	cAuthClientMessageHandler* handler = cAuthClientMessageHandler::getInstance();

	do {

		//clear the old char buffer
		for (int i = 0; i < DEFAULT_BUFLEN; ++i) {
			this->recvbuf[i] = '\0';
		}

		result = recv(this->connectSocket, this->recvbuf, this->recvbuflen, 0);
		if (result > 0)
		{
			cBuffer tempBuff;
			tempBuff.loadBuffer(this->recvbuf, result);

			this->ourRecvBuffer.appendToBuffer(tempBuff);

			handler->handleMessage(this->ourRecvBuffer, this->connectSocket);

		}
		else if (result == 0)
		{
			printf("Connection closed\n");
		}
		else
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
		}

	} while (result > 0);
}

/*
*	sendMessage()
*	This function sends a message buffer to the server.
*	Accepts: A cBuffer object containing the message protocol.
*	Returns: A bool representing whether the winsock send call was successful.
*/
bool cTCPAuthClient::sendMessage(cBuffer messageBuffer) {
	// Step #4 Send the message to the server

	int result = send(this->connectSocket, messageBuffer.toCharArray(), messageBuffer.getLength(), 0);
	if (result == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(this->connectSocket);
		WSACleanup();
		return false;
	}

	return true;
}

/*
*	closeSendSocket()
*	This function shuts down the connected socket.
*	Accepts: None
*	Returns: A bool representing whether the winsock shutdown call was successful.
*/
bool cTCPAuthClient::closeSendSocket() {

	// shutdown the connection since no more data will be sent
	int result = shutdown(this->connectSocket, SD_BOTH);
	if (result == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(this->connectSocket);
		WSACleanup();
		return false;
	}

	return true;
}

/*
*	cleanUp()
*	This function closes the socket and cleans up winsock.
*	Accepts: None
*	Returns: None
*/
void cTCPAuthClient::cleanUp() {
	closesocket(this->connectSocket);
	WSACleanup();
}