#include "cMessageProtocolHelper.h"

/*
*	createServerJoinMessage()
*	This function creates the message buffer to send the join server message.
*	Accepts: A string representing the username.
*	Returns: A cBuffer with the correct message protocol format.
*/
cBuffer cMessageProtocolHelper::createServerJoinMessage(std::string username, std::string password) {
	cBuffer msgBuffer;

	msgBuffer.setWriteIndex(8);
	msgBuffer.writeIntBE((int)username.size());
	msgBuffer.writeStringBE(username);

	msgBuffer.writeIntBE((int)password.size());
	msgBuffer.writeStringBE(password);

	this->addMessageHeader(msgBuffer, 0);

	return msgBuffer;
}

/*
*	createJoinRoomMessage()
*	This function creates the message buffer to send the join room message (/join command).
*	Accepts: A string representing the room name.
*	Returns: A cBuffer with the correct message protocol format.
*/
cBuffer cMessageProtocolHelper::createJoinRoomMessage(std::string roomName) {

	cBuffer msgBuffer;

	msgBuffer.setWriteIndex(8);
	msgBuffer.writeIntBE((int)roomName.size());
	msgBuffer.writeStringBE(roomName);

	this->addMessageHeader(msgBuffer, 1);

	return msgBuffer;
}

/*
*	createSendMessage()
*	This function creates the message buffer to send a new chat message to the server
	to be broadcast to users. (/say command)
*	Accepts: 
		- A string representing the room name.
		- A string representing the message.
*	Returns: A cBuffer with the correct message protocol format.
*/
cBuffer cMessageProtocolHelper::createSendMessage(std::string roomName, std::string message) {

	cBuffer msgBuffer;

	msgBuffer.setWriteIndex(8);
	msgBuffer.writeIntBE((int)roomName.size());
	msgBuffer.writeStringBE(roomName);

	msgBuffer.writeIntBE((int)message.size());
	msgBuffer.writeStringBE(message);

	this->addMessageHeader(msgBuffer, 2);

	return msgBuffer;
}

/*
*	createRecieveMessage()
*	This function creates the message buffer to send to the server so it knows 
	where to broadcast a new message.
*	Accepts: 
		- A string representing the username.
		- A string representing the new message.
		- A string representing the room name.
*	Returns: A cBuffer with the correct message protocol format.
*/
cBuffer cMessageProtocolHelper::createRecieveMessage(std::string username, std::string message, std::string roomName) {
	cBuffer msgBuffer;

	msgBuffer.setWriteIndex(8);
	msgBuffer.writeIntBE((int)username.size());
	msgBuffer.writeStringBE(username);

	msgBuffer.writeIntBE((int)message.size());
	msgBuffer.writeStringBE(message);

	msgBuffer.writeIntBE((int)roomName.size());
	msgBuffer.writeStringBE(roomName);

	this->addMessageHeader(msgBuffer, 3);

	return msgBuffer;
}

/*
*	createLeaveRoomMessage()
*	This function creates the message buffer to send the leave room command to the server 
	(/leave command).
*	Accepts: A string representing the room name.
*	Returns: A cBuffer with the correct message protocol format.
*/
cBuffer cMessageProtocolHelper::createLeaveRoomMessage(std::string roomName) {
	cBuffer msgBuffer;

	msgBuffer.setWriteIndex(8);
	msgBuffer.writeIntBE((int)roomName.size());
	msgBuffer.writeStringBE(roomName);

	this->addMessageHeader(msgBuffer, 4);

	return msgBuffer;
}

/*
*	createUserLeftRoomMessage()
*	This function creates the message buffer to send to the server to that it knows who to broadcast to 
	that a user left a room.
*	Accepts: 
		- A string representing the room name.
		- A string representing the username.
*	Returns: A cBuffer with the correct message protocol format.
*/
cBuffer cMessageProtocolHelper::createUserLeftRoomMessage(std::string roomName, std::string username) {
	cBuffer msgBuffer;

	msgBuffer.setWriteIndex(8);
	msgBuffer.writeIntBE((int)username.size());
	msgBuffer.writeStringBE(username);

	msgBuffer.writeIntBE((int)roomName.size());
	msgBuffer.writeStringBE(roomName);

	this->addMessageHeader(msgBuffer, 5);

	return msgBuffer;
}

/*
*	createUserJoinRoomMessage()
*	This function creates the message buffer to send to the server to that it knows who to broadcast to 
	that a user joined a room.
*	Accepts: 
		- A string representing the username.
		- A string representing the room name.
*	Returns: A cBuffer with the correct message protocol format.
*/
cBuffer cMessageProtocolHelper::createUserJoinRoomMessage(std::string username, std::string roomname) {
	cBuffer msgBuffer;

	msgBuffer.setWriteIndex(8);
	msgBuffer.writeIntBE((int)username.size());
	msgBuffer.writeStringBE(username);

	msgBuffer.writeIntBE((int)roomname.size());
	msgBuffer.writeStringBE(roomname);

	this->addMessageHeader(msgBuffer, 6);

	return msgBuffer;
}

/*
*	addMessageHeader()
*	This function creates the mesage header for the packet.
*	Accepts:
		- A cBuffer representing the buffer to add the header to.
		- An integer representing the message ID.
*	Returns: None
*/
void cMessageProtocolHelper::addMessageHeader(cBuffer& msgBuffer, int messageId) {

	int oldWriteIndex = msgBuffer.getWriteIndex();
	int msgLen = msgBuffer.getLength();

	// put message header in
	msgBuffer.setWriteIndex(0);
	msgBuffer.writeIntBE(msgLen);
	msgBuffer.writeIntBE(messageId); // Message type Id

	// reset write index
	msgBuffer.setWriteIndex(oldWriteIndex);
}


cBuffer cMessageProtocolHelper::createUserAuthSuccessMessage(int user_id, std::string created_date) {
	cBuffer msgBuffer;

	msgBuffer.setWriteIndex(8);
	msgBuffer.writeIntBE(user_id);

	msgBuffer.writeIntBE((int)created_date.size());
	msgBuffer.writeStringBE(created_date);

	this->addMessageHeader(msgBuffer, 7);

	return msgBuffer;
}

cBuffer cMessageProtocolHelper::createUserAuthFailMessage(int failReason) {
	cBuffer msgBuffer;

	msgBuffer.setWriteIndex(8);
	msgBuffer.writeIntBE(failReason);

	this->addMessageHeader(msgBuffer, 8);

	return msgBuffer;
}

cBuffer cMessageProtocolHelper::createRegisterMessage(std::string username, std::string password) {
	cBuffer msgBuffer;

	msgBuffer.setWriteIndex(8);
	msgBuffer.writeIntBE((int)username.size());
	msgBuffer.writeStringBE(username);

	msgBuffer.writeIntBE((int)password.size());
	msgBuffer.writeStringBE(password);

	this->addMessageHeader(msgBuffer, 8);

	return msgBuffer;
}

cBuffer cMessageProtocolHelper::createRegisterSuccessMessage(int userId) {
	cBuffer msgBuffer;

	msgBuffer.setWriteIndex(8);
	msgBuffer.writeIntBE(userId);
	
	this->addMessageHeader(msgBuffer, 9);

	return msgBuffer;
}

cBuffer cMessageProtocolHelper::createRegisterFailMessage(int reason) {
	cBuffer msgBuffer;

	msgBuffer.setWriteIndex(8);
	msgBuffer.writeIntBE(reason);

	this->addMessageHeader(msgBuffer, 10);

	return msgBuffer;
}