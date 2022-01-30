#pragma once

#include <winsock2.h>
#include <sstream>
#include <map>
#include <vector>

#include "../include/cMessageProtocolHelper.h"
#include "../include/cBuffer.h"
#include "../include/cConsole.h"

class cClientMessageHandler {
public:
	std::vector<std::string> messageHistory;
	std::map<SOCKET, std::string> connectedUsers;
	std::map<std::string, std::vector<SOCKET>> channelUserMap;

	int currentRow;

	static cClientMessageHandler* getInstance();
	cClientMessageHandler(cClientMessageHandler const&) = delete;
	void operator=(cClientMessageHandler const&) = delete;
	void handleMessage(cBuffer& buffer, SOCKET socketId);
	void updateUI();
	
private:
	cClientMessageHandler();
	~cClientMessageHandler();	
};

