#include "cProtoBufHelper.h"

/*
*	createAuthenticateWebMessage()
*	This function creates a buffer after deserializing a Google Protocol Buffer containing the required 
*	information for a successful authentication.
*	Accepts:
*		- A string representing the users email.
*		- A string representing the users password
*		- An int representing the client socket ID.
*	Returns: A cBuffer containing the information.
*/
cBuffer cProtoBufHelper::createAuthenticateWebMessage(std::string email, std::string password, int socketId) {

	cBuffer messageBuffer;

	// Create the Google protocol buffer and set the required information
	AuthenticateWeb web;
	web.set_requestid(socketId);
	web.set_email(email);
	web.set_plaintextpassword(password);
	
	// Serialize proto buffer
	std::string serialized;
	web.SerializeToString(&serialized);

	// Add packet header
	messageBuffer.writeIntBE(serialized.size() + 8);
	messageBuffer.writeIntBE(1);

	// Add protobuff
	messageBuffer.writeStringBE(serialized);

	return messageBuffer;
}

/*
*	createAuthenticateWebSuccess()
*	This function creates a buffer after deserializing a Google Protocol Buffer containing the required
*	information for a successful authentication message back to the chat client.
*	Accepts:
*		- A string representing the users email.
*		- An int representing the user ID.
*		- A string representing the creation date of the user.
*		- An int representing the client socket ID.
*	Returns: A cBuffer containing the information.
*/
cBuffer cProtoBufHelper::createAuthenticateWebSuccess(std::string email, int user_id, std::string createdAtDate, int socketId) {
	cBuffer messageBuffer;

	// Create the Google protocol buffer and set the required information
	AuthenticateWebSuccess web;
	web.set_email(email);
	web.set_requestid(socketId);
	web.set_userid(user_id);
	web.set_creationdate(createdAtDate);

	// Serialize proto buffer
	std::string serialized;
	web.SerializeToString(&serialized);

	// Add packet header
	messageBuffer.writeIntBE(serialized.size() + 8);
	messageBuffer.writeIntBE(2);

	// Add protobuff
	messageBuffer.writeStringBE(serialized);

	return messageBuffer;
}

/*
*	createAuthenticateWebFail()
*	This function creates a buffer after deserializing a Google Protocol Buffer containing the required
*	information for a unsuccessful authentication message back to the chat client.
*	Accepts:
*		- A Google Protocol Buffer enum representing the failure reason.
*		- An int representing the client socket ID.
*	Returns: A cBuffer containing the information.
*/
cBuffer cProtoBufHelper::createAuthenticateWebFail(AuthenticateWebFailure::eReason reason, int socketId) {

	cBuffer messageBuffer;

	AuthenticateWebFailure web;
	web.set_reason(reason);
	web.set_requestid(socketId);

	// Serialize proto buffer
	std::string serialized;
	web.SerializeToString(&serialized);

	// Add packet header
	messageBuffer.writeIntBE(serialized.size() + 8);
	messageBuffer.writeIntBE(3);

	// Add protobuff
	messageBuffer.writeStringBE(serialized);

	return messageBuffer;
}

/*
*	createRegisterAccount()
*	This function creates a buffer after deserializing a Google Protocol Buffer containing the required
*	information for a register user message back to the chat client.
*	Accepts:
*		- A string representing the users email.
*		- A string representing the users password.
*		- An int representing the client socket ID.
*	Returns: A cBuffer containing the information.
*/
cBuffer cProtoBufHelper::createRegisterAccount(std::string email, std::string password, int socketId) {
	cBuffer messageBuffer;

	CreateAccountWeb web;
	web.set_email(email);
	web.set_plaintextpassword(password);
	web.set_requestid(socketId);

	// Serialize proto buffer
	std::string serialized;
	web.SerializeToString(&serialized);

	// Add packet header
	messageBuffer.writeIntBE(serialized.size() + 8);
	messageBuffer.writeIntBE(4);

	// Add protobuff
	messageBuffer.writeStringBE(serialized);

	return messageBuffer;
}

/*
*	createRegisterAccountSuccess()
*	This function creates a buffer after deserializing a Google Protocol Buffer containing the required
*	information for a successful registration message back to the chat client.
*	Accepts:
*		- An int representing the user ID.
*		- An int representing the client socket ID.
*	Returns: A cBuffer containing the information.
*/
cBuffer cProtoBufHelper::createRegisterAccountSuccess(int user_id, int socketId) {
	cBuffer messageBuffer;

	CreateAccountWebSuccess web;
	web.set_userid(user_id);
	web.set_requestid(socketId);

	// Serialize proto buffer
	std::string serialized;
	web.SerializeToString(&serialized);

	// Add packet header
	messageBuffer.writeIntBE(serialized.size() + 8);
	messageBuffer.writeIntBE(5);

	// Add protobuff
	messageBuffer.writeStringBE(serialized);

	return messageBuffer;
}

/*
*	createRegisterAccountFail()
*	This function creates a buffer after deserializing a Google Protocol Buffer containing the required
*	information for a unsuccessful registration message back to the chat client.
*	Accepts:
*		- A Google Protocol Buffer enum representing the failure reason.
*		- An int representing the client socket ID.
*	Returns: A cBuffer containing the information.
*/
cBuffer cProtoBufHelper::createRegisterAccountFail(CreateAccountWebFailure::eReason reason, int socketId) {
	cBuffer messageBuffer;

	CreateAccountWebFailure web;
	web.set_reason(reason);
	web.set_requestid(socketId);

	// Serialize proto buffer
	std::string serialized;
	web.SerializeToString(&serialized);

	// Add packet header
	messageBuffer.writeIntBE(serialized.size() + 8);
	messageBuffer.writeIntBE(6);

	// Add protobuff
	messageBuffer.writeStringBE(serialized);

	return messageBuffer;
}