#pragma once

#include<stdio.h>
#include<winsock2.h>

#include "../include/cBuffer.h"

#pragma comment(lib, "ws2_32.lib") //Winsock Library

class cTCPServer {
public:
	cTCPServer();
	~cTCPServer();

	static const int MAX_CLIENTS = 30;
	static const int MAX_RECV = 512; //size of our receive buffer
	char *buffer;

	WSADATA winsockData;
	fd_set readfds; //set of socket descriptors

	SOCKET masterSocket;
	SOCKET new_socket; 
	SOCKET client_sockets[MAX_CLIENTS]; 
	SOCKET currentSocket;
	
	cBuffer recvBuffer[MAX_CLIENTS];

	struct sockaddr_in serverAddressInfo;   // server IP info
	struct sockaddr_in clientAddress;		// current client IP info
	
	// result variables
	int activity;
	int	addressLength; 
	int valread;
	
	bool initializeWinsock();
	bool createMasterSocket();
	void destroySocket();
	bool bindSocket();
	void startListen();
	void cleanUp();
};

