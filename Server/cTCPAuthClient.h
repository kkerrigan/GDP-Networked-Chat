#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include "../include/cBuffer.h"

#define DEFAULT_BUFLEN 512					// Default buffer length of our buffer in characters
#define DEFAULT_PORT "8889"					// The default port to use
#define SERVER "127.0.0.1"					// The IP of our server

class cTCPAuthClient
{
public:
	cTCPAuthClient();
	~cTCPAuthClient();

	WSADATA winsockData;					// holds Winsock data
	SOCKET connectSocket;					// Our connection socket used to connect to the server

	struct addrinfo *infoResult;			// Holds the address information of our server
	struct addrinfo *connectAttempt;		// Current address attempting to join 
	struct addrinfo hints;

	// Static buffer sent to winsock recv() call
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	cBuffer ourRecvBuffer;

	bool initializeWinsock();
	bool resolveServerAddr();
	bool connectToServer();
	void startListen();
	bool sendMessage(cBuffer messageBuffer);
	bool closeSendSocket();
	void cleanUp();
};

