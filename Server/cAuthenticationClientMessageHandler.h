#pragma once

#include <winsock2.h>
#include <map>
#include <vector>

#include "../include/cBuffer.h"

class cAuthClientMessageHandler {
public:
	std::map<SOCKET, std::string> connectedUsers;
	std::map<std::string, std::vector<SOCKET>> roomUserMap;

	static cAuthClientMessageHandler* getInstance();
	cAuthClientMessageHandler(cAuthClientMessageHandler const&) = delete;
	void operator=(cAuthClientMessageHandler const&) = delete;
	void removeUserOnDisconnect(SOCKET socket);
	void handleMessage(cBuffer& buffer, SOCKET socketId);

private:
	cAuthClientMessageHandler();
	~cAuthClientMessageHandler();
};