#pragma once
#include <string>
#include "../include/cBuffer.h"

class cMessageProtocolHelper {

public:
	cBuffer createServerJoinMessage(std::string username, std::string password);
	cBuffer createJoinRoomMessage(std::string roomName);
	cBuffer createSendMessage(std::string roomName, std::string message);
	cBuffer createRecieveMessage(std::string username, std::string message, std::string roomName);
	cBuffer createLeaveRoomMessage(std::string roomName);
	cBuffer createUserLeftRoomMessage(std::string roomName, std::string username);
	cBuffer createUserJoinRoomMessage(std::string username, std::string roomname);
	cBuffer createUserAuthSuccessMessage(int user_id, std::string created_date);
	cBuffer createUserAuthFailMessage(int failReason);
	cBuffer createRegisterMessage(std::string username, std::string password);
	cBuffer createRegisterSuccessMessage(int userId);
	cBuffer createRegisterFailMessage(int reason);
	void addMessageHeader(cBuffer& msgBuffer, int messageId);
};
