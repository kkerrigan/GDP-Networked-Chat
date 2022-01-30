#ifndef _cAuthServerMessageHandler_HG_
#define _cAuthServerMessageHandler_HG_

#include <winsock2.h>
#include <map>
#include <vector>

#include "../include/cBuffer.h"

class cAuthServerMessageHandler {
public:
	std::map<SOCKET, std::string> connectedUsers;
	std::map<std::string, std::vector<SOCKET>> roomUserMap;

	static cAuthServerMessageHandler* getInstance();
	cAuthServerMessageHandler(cAuthServerMessageHandler const&) = delete;
	void operator=(cAuthServerMessageHandler const&) = delete;
	void removeUserOnDisconnect(SOCKET socket);
	void handleMessage(cBuffer& buffer, SOCKET socketId);

private:
	cAuthServerMessageHandler();
	~cAuthServerMessageHandler();
};

#endif