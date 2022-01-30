#include "cServerMessageHandler.h"
#include "../include/cMessageProtocolHelper.h"
#include "cTCPAuthClient.h"
#include "../AuthenticationServer/cProtoBufHelper.h"

extern cTCPAuthClient authClient;
std::map<SOCKET, std::string> g_connectedUsers;

// c'tor
cServerMessageHandler::cServerMessageHandler(){}

// d'tor
cServerMessageHandler::~cServerMessageHandler(){}

/*
*	getInstance()
*	This function contains the static variable for the Singleton.
*	Accepts: None
*	Returns: A pointer to the cServerMessageHandler class.
*/
cServerMessageHandler* cServerMessageHandler::getInstance() {
	static cServerMessageHandler instance;
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
void cServerMessageHandler::handleMessage(cBuffer& buffer, SOCKET socketId) {
	cMessageProtocolHelper helper;
	cBuffer responseMessage;

	// header size on a packet
	int packetSize = buffer.readIntBE();
	int messageId = buffer.readIntBE();

	if ((int)buffer.getLength() >= (packetSize - 8)) {
		switch (messageId) {
		case 0: // client joined the server after entering username
		{
			int usernameLen = buffer.readIntBE();
			std::string username = buffer.readStringBE(usernameLen);

			int passwordLen = buffer.readIntBE();
			std::string password = buffer.readStringBE(passwordLen);

			cProtoBufHelper protoHelper;
			cBuffer buff = protoHelper.createAuthenticateWebMessage(username, password, socketId);

			authClient.sendMessage(buff);

			break;
		}

		case 1: // user joined a room using the /join command
		{
			int roomNameLen = buffer.readIntBE();
			std::string roomName = buffer.readStringBE(roomNameLen);
			printf("Join room: %s\n", roomName.c_str());

			responseMessage = helper.createUserJoinRoomMessage(g_connectedUsers[socketId], roomName);

			//check if the user is already connected
			std::map<std::string, std::vector<SOCKET>>::iterator itChannel = this->roomUserMap.find(roomName);

			//if the room does not exist create it
			if (itChannel == this->roomUserMap.end()) {
				this->roomUserMap[roomName];
			}

			this->roomUserMap[roomName].push_back(socketId);

			//send the message to everyone in the room
			for (auto userInChannel = (this->roomUserMap[roomName]).begin(); userInChannel != (this->roomUserMap[roomName]).end(); ++userInChannel) {
				send(*userInChannel, responseMessage.toCharArray(), (int)responseMessage.getLength(), 0);
			}

			break;
		}

		case 2: // server sends the users message to everyone in their room
		{

			int roomNameLen = buffer.readIntBE();
			std::string roomName = buffer.readStringBE(roomNameLen);
			printf("Send message to room %s from %s\n", roomName.c_str(), g_connectedUsers[socketId].c_str());

			int messageLen = buffer.readIntBE();
			std::string message = buffer.readStringBE(messageLen);
			printf("Message: %s\n", message.c_str());

			//get the users name
			std::string username = g_connectedUsers[socketId];

			responseMessage = helper.createRecieveMessage(username, roomName, message);

			//send the message to everyone in the room
			for (auto userInChannel = (this->roomUserMap[roomName]).begin(); userInChannel != (this->roomUserMap[roomName]).end(); ++userInChannel) {
				send(*userInChannel, responseMessage.toCharArray(), (int)responseMessage.getLength(), 0);
			}

			break;
		}

		case 3:
		{
			//NOT IMPLIMENTED ON SERVER	
			break;
		}

		case 4: // user left a room using the /leave command
		{
			int roomNameLen = buffer.readIntBE();
			std::string roomName = buffer.readStringBE(roomNameLen);
			printf("Leave room: %s\n", roomName.c_str());

			//check if the user is already connected
			std::map<std::string, std::vector<SOCKET>>::iterator itChannel = this->roomUserMap.find(roomName);

			//if the room does not exist create it
			if (itChannel == this->roomUserMap.end()) {
				return;
			}

			std::string username = g_connectedUsers[socketId];
			responseMessage = helper.createUserLeftRoomMessage(roomName, username);

			//tell the everyone that a person has left
			for (auto userInChannel = (this->roomUserMap[roomName]).begin(); userInChannel != (this->roomUserMap[roomName]).end(); ++userInChannel) {
				send(*userInChannel, responseMessage.toCharArray(), (int)responseMessage.getLength(), 0);
			}

			for (auto userInChannel = (this->roomUserMap[roomName]).begin(); userInChannel != (this->roomUserMap[roomName]).end(); ++userInChannel) {
				if (*userInChannel == socketId) {
					this->roomUserMap[roomName].erase(userInChannel);
					break;
				}
			}

			//delete the room if it is empty and not "general"
			if (roomName != "General" && this->roomUserMap[roomName].size() == 0) {
				this->roomUserMap.erase(itChannel);
			}

			break;
		}

		case 5:
		{
			//NOT IMPLIMENTED ON SERVER	
		}

		case 6:
		{
			//NOT IMPLIMENTED ON SERVER	
		}

		case 7:
		{
			//NOT IMPLIMENTED ON SERVER	
		}

		case 8:
		{
			int usernameLen = buffer.readIntBE();
			std::string username = buffer.readStringBE(usernameLen);

			int passwordLen = buffer.readIntBE();
			std::string password = buffer.readStringBE(passwordLen);

			//TODO: authenticate
			cProtoBufHelper protoHelper;
			cBuffer buff = protoHelper.createRegisterAccount(username, password, socketId);

			authClient.sendMessage(buff);
		}

		}

		//get rid of the data we have used
		buffer.flushBuffer();

		if (buffer.getLength() >= 8) {
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
void cServerMessageHandler::removeUserOnDisconnect(SOCKET socket) {
	std::map<SOCKET, std::string>::iterator it = g_connectedUsers.find(socket);

	if (it != g_connectedUsers.end()) {
		g_connectedUsers.erase(it);
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