#pragma once

#include <winsock2.h>
#include <map>
#include <vector>

#include "../include/cBuffer.h"

extern std::map<SOCKET, std::string> g_connectedUsers;

class cServerMessageHandler {
public:
	std::map<std::string, std::vector<SOCKET>> roomUserMap;

	static cServerMessageHandler* getInstance();
	cServerMessageHandler(cServerMessageHandler const&) = delete;
	void operator=(cServerMessageHandler const&) = delete;
	void removeUserOnDisconnect(SOCKET socket);
	void handleMessage(cBuffer& buffer, SOCKET socketId);

private:
	cServerMessageHandler();
	~cServerMessageHandler();
};

