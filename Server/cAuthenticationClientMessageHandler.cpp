#include "../include/cMessageProtocolHelper.h"
#include "cAuthenticationClientMessageHandler.h"
#include "../AuthenticationServer/cProtoBufHelper.h"
#include "../AuthenticationServer/AuthenticationProtocol.pb.h"

#define MYSQL_HOST		"127.0.0.1:3306"
#define MYSQL_USER		"root"
#define MYSQL_PASSWORD	"password"
#define MYSQL_SCHEMA	"INFO6016_authentication"

extern std::map<SOCKET, std::string> g_connectedUsers;

// c'tor
cAuthClientMessageHandler::cAuthClientMessageHandler() {}

// d'tor
cAuthClientMessageHandler::~cAuthClientMessageHandler() {}

/*
*	getInstance()
*	This function contains the static variable for the Singleton.
*	Accepts: None
*	Returns: A pointer to the cServerMessageHandler class.
*/
cAuthClientMessageHandler* cAuthClientMessageHandler::getInstance() {
	static cAuthClientMessageHandler instance;
	return &instance;
}

/*
*	handleMessage()
*	This function handles recieving messages from individual clients and then sending them back to the
	appropriate clients.
*	Accepts:
*		- A cBuffer object containing the message to be sent to the clients.
*		- A SOCKET object representing which user the message came from.
*	Returns: None.
*/
void cAuthClientMessageHandler::handleMessage(cBuffer& buffer, SOCKET socketId) {
	cMessageProtocolHelper helper;
	cProtoBufHelper protoHelper;
	cBuffer responseMessage;

	// header size on a packet
	int packetSize = buffer.readIntBE();

	//make sure we have all the data
	if ((int)buffer.getLength() >= (packetSize - 8)) {

		//insert proto buf code here
		// deserialize and process message

		int messageType = buffer.readIntBE();

		switch (messageType) {
		case 1:
			//not implimented on client
			break;

		case 2:
		{
			AuthenticateWebSuccess suc;
			suc.ParseFromArray((buffer.readStringBE((int)buffer.getLength()).c_str()), (int)buffer.getLength());

			//TODO: respond to client
			cBuffer response = helper.createUserAuthSuccessMessage(suc.userid(), suc.creationdate());
			send(suc.requestid(), response.toCharArray(), response.getLength(), 0);

			printf("User joined the server: %s\n", suc.email().c_str());

			//check if the user is already connected
			std::map<SOCKET, std::string>::iterator itConnectedUser = g_connectedUsers.find(suc.requestid());

			if (itConnectedUser == g_connectedUsers.end()) {
				g_connectedUsers[suc.requestid()] = suc.email();
			}

			break;
		}
			

		case 3:
		{
			AuthenticateWebFailure failMess;
			failMess.ParseFromArray((buffer.readStringBE((int)buffer.getLength()).c_str()), (int)buffer.getLength());

			cBuffer response = helper.createUserAuthFailMessage(failMess.reason());
			send(failMess.requestid(), response.toCharArray(), response.getLength(), 0);

			break;
		}

		case 5:
		{
			CreateAccountWebSuccess suc;
			suc.ParseFromArray((buffer.readStringBE((int)buffer.getLength()).c_str()), (int)buffer.getLength());

			cBuffer response = helper.createRegisterSuccessMessage(suc.userid());
			send(suc.requestid(), response.toCharArray(), response.getLength(), 0);

			break;
		}

		case 6:
		{
			CreateAccountWebFailure suc;
			suc.ParseFromArray((buffer.readStringBE((int)buffer.getLength()).c_str()), (int)buffer.getLength());

			cBuffer response = helper.createRegisterFailMessage(suc.reason());
			send(suc.requestid(), response.toCharArray(), response.getLength(), 0);

			break;
		}
			
		}

		//get rid of the data we have used
		buffer.flushBuffer();

		if (buffer.getLength() >= 4) {
			this->handleMessage(buffer, socketId);
		}
	}
}

/*
*	removeUserOnDisconnect()
*	This function removes the user from our list of connected users once they disconnect from the server.
*	Accepts: A SOCKET object representing the user to be removed.
*	Returns: None
*/
void cAuthClientMessageHandler::removeUserOnDisconnect(SOCKET socket) {
	std::map<SOCKET, std::string>::iterator it = this->connectedUsers.find(socket);

	if (it != this->connectedUsers.end()) {
		this->connectedUsers.erase(it);
	}

	for (std::map<std::string, std::vector<SOCKET>>::iterator it = this->roomUserMap.begin(); it != this->roomUserMap.end(); ++it) {
		for (std::vector<SOCKET>::iterator vecIt = it->second.begin(); vecIt != it->second.end(); ++vecIt) {
			if (*vecIt == socket) {
				it->second.erase(vecIt);
				break;
			}
		}
	}

}