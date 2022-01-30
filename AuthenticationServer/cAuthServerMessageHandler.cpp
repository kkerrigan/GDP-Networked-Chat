#include "cAuthServerMessageHandler.h"
#include "cProtoBufHelper.h"
#include "cDatabaseHelper.h"
#include "cTCPAuthServer.h"
#include "cHashGenerator.h"

#define MYSQL_HOST		"127.0.0.1:3306"
#define MYSQL_USER		"root"
#define MYSQL_PASSWORD	"password"
#define MYSQL_SCHEMA	"INFO6016_authentication"

extern cAuthTCPServer tcpAuthServer;

// c'tor
cAuthServerMessageHandler::cAuthServerMessageHandler() {}

// d'tor
cAuthServerMessageHandler::~cAuthServerMessageHandler() {}

/*
*	getInstance()
*	This function contains the static variable for the Singleton.
*	Accepts: None
*	Returns: A pointer to the cServerMessageHandler class.
*/
cAuthServerMessageHandler* cAuthServerMessageHandler::getInstance() {
	static cAuthServerMessageHandler instance;
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
void cAuthServerMessageHandler::handleMessage(cBuffer& buffer, SOCKET socketId) {
	cProtoBufHelper helper;
	cBuffer responseMessage;

	// header size on a packet
	int packetSize = buffer.readIntBE();
	
	//make sure we have all the data
	if ((int)buffer.getLength() >= (packetSize - 8)) {
		
		//insert proto buf code here
		// deserialize and process message
	
		int messageType = buffer.readIntBE();

		switch (messageType) {
		case 1: // Authenticate user login
		{
			AuthenticateWeb auth;
			auth.ParseFromArray((buffer.readStringBE((int)buffer.getLength()).c_str()), (int)buffer.getLength());

			int requestId = auth.requestid();
			std::string email = auth.email();
			std::string plainTextPass = auth.plaintextpassword();

			cDatabaseHelper dbHelper;
			if (!dbHelper.ConnectToDatabase(MYSQL_HOST, MYSQL_USER, MYSQL_PASSWORD, MYSQL_SCHEMA)) {
				std::cout << "Cannot connect to mysql" << std::endl;

				//server error
				cBuffer response = helper.createAuthenticateWebFail(AuthenticateWebFailure::eReason::AuthenticateWebFailure_eReason_INTERNAL_SERVER_ERROR, requestId);
				send(socketId, response.toCharArray(), response.getLength(), 0);

			}
			else {
				std::cout << "Connected to mysql" << std::endl;
			}

			if (dbHelper.verifyPassword(email, plainTextPass)) {

				int user_id;
				std::string create_date;
				dbHelper.getUserIdWithCreateDate(email, &user_id, &create_date);

				//valid pass
				cBuffer response = helper.createAuthenticateWebSuccess(email, user_id, create_date, requestId);
				send(socketId, response.toCharArray(), response.getLength(), 0);

				std::cout << "valid credentials" << std::endl;
			}
			else {
				cBuffer response = helper.createAuthenticateWebFail(AuthenticateWebFailure::eReason::AuthenticateWebFailure_eReason_INVALID_CREDENTIALS, requestId);
				send(socketId, response.toCharArray(), response.getLength(), 0);

				//invalid
				std::cout << "invalid credentials" << std::endl;
			}

			break;
		}
			

		case 4: // Registering a new user
		{
			CreateAccountWeb auth;
			auth.ParseFromArray((buffer.readStringBE((int)buffer.getLength()).c_str()), (int)buffer.getLength());

			int requestId = auth.requestid();
			std::string email = auth.email();
			std::string plainTextPass = auth.plaintextpassword();

			//check if the username exists
			cDatabaseHelper dh;
			dh.ConnectToDatabase(MYSQL_HOST, MYSQL_USER, MYSQL_PASSWORD, MYSQL_SCHEMA);

			if (dh.isUserExist(email)) {
				cBuffer response = helper.createRegisterAccountFail(CreateAccountWebFailure::eReason::CreateAccountWebFailure_eReason_ACCOUNT_ALREADY_EXISTS, requestId);
				send(socketId, response.toCharArray(), response.getLength(), 0);
			}

			//shitty password rules
			if (plainTextPass.size() < 4 || plainTextPass.size() > 255) {
				cBuffer response = helper.createRegisterAccountFail(CreateAccountWebFailure::eReason::CreateAccountWebFailure_eReason_INVALID_PASSWORD, requestId);
				send(socketId, response.toCharArray(), response.getLength(), 0);
			}

			char salt[SALT_LENGTH] = { 0 };
			char passwordHash[HASH_LENGTH] = { 0 };

			cHashGenerator hGen;

			// create random salt
			hGen.generateRandomSalt(salt);

			// hash the salt and user text together
			std::string combined;
			combined.append(salt);
			combined.append(plainTextPass.c_str());

			hGen.generateHashFromPlainText(combined.c_str(), passwordHash);

			//create the account
			if (dh.createUser(email, salt, passwordHash)) {

				int user_id;
				std::string createDate;
				dh.getUserIdWithCreateDate(email, &user_id, &createDate);

				cBuffer response = helper.createRegisterAccountSuccess(user_id, requestId);
				send(socketId, response.toCharArray(), response.getLength(), 0);
			}
			else {
				cBuffer response = helper.createRegisterAccountFail(CreateAccountWebFailure::eReason::CreateAccountWebFailure_eReason_INTERNAL_SERVER_ERROR, requestId);
				send(socketId, response.toCharArray(), response.getLength(), 0);
			}

			break;
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
void cAuthServerMessageHandler::removeUserOnDisconnect(SOCKET socket) {
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