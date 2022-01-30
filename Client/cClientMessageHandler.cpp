#include "cClientMessageHandler.h"
void detachCIN();
void promptMenu(int rows, int columns, std::string errorMsg);

// c'tor
cClientMessageHandler::cClientMessageHandler() {
	this->currentRow = 1;
};

// d'tor
cClientMessageHandler::~cClientMessageHandler() {};

/*
*	getInstance()
*	This function contains the static variable for the Singleton.
*	Accepts: None
*	Returns: A pointer to the cClientMessageHandler class.
*/
cClientMessageHandler* cClientMessageHandler::getInstance() {
	static cClientMessageHandler instance;

	return &instance;
}

/*
*	handleMessage()
*	This function determines which message was created in the Message Protocol Helper and sends it to
	the client or server.
*	Accepts:
*		- A cBuffer object containing the message to be sent to the server
*		- A SOCKET object representing which user the message came from.
*	Returns: None.
*/
void cClientMessageHandler::handleMessage(cBuffer& buffer, SOCKET socketId) {
	
	cConsole console;
	cMessageProtocolHelper helper;
	cBuffer responseMessage; // buffer to be sent after processing

	// size of the header
	int packetSize = buffer.readIntBE();
	int messageId = buffer.readIntBE();

	
	if ((int)buffer.getLength() >= (packetSize - 8)) {
		switch (messageId) {
		case 0: // client connected to the server after entering username
		{
			// NO LONGER IMPLEMENTED ON CLIENT, NOW ON AUTH SERVER
			break;
		}

		case 1: // User joined a room using the /join command
		{
			// read the room name/length from the buffer
			int roomNameLen = buffer.readIntBE();
			std::string roomName = buffer.readStringBE(roomNameLen);

			//check if the user is already connected
			std::map<std::string, std::vector<SOCKET>>::iterator itChannel = this->channelUserMap.find(roomName);

			//if the room does not exist create it
			if (itChannel == this->channelUserMap.end()) {
				this->channelUserMap[roomName];
			}

			this->channelUserMap[roomName].push_back(socketId);

			// send join to the server
			responseMessage = helper.createUserJoinRoomMessage(this->connectedUsers[socketId], roomName);

			break;
		}

		case 2: // client sends their message to server using say command
		{
			responseMessage = helper.createJoinRoomMessage("User send message.");
			int roomNameLen = buffer.readIntBE();
			std::string roomName = buffer.readStringBE(roomNameLen);

			int messageLen = buffer.readIntBE();
			std::string message = buffer.readStringBE(messageLen);

			break;
		}

		case 3: // server recieves the message and sends it to all users in the roomn
		{
			// read username, rooom and message from buffer
			int usernameLen = buffer.readIntBE();
			std::string username = buffer.readStringBE(usernameLen);

			int messageLen = buffer.readIntBE();
			std::string message = buffer.readStringBE(messageLen);

			int roomNameLen = buffer.readIntBE();
			std::string roomName = buffer.readStringBE(roomNameLen);

			// add new message to message history
			std::string resultMsgs = message + "-" + username + ": " + roomName;
			messageHistory.push_back(resultMsgs);

			break;
		}
		case 4:
		{
			//NOT IMPLIMENTED ON CLIENT
			break;
		}
		case 5: // server informs other users when someone leaves a room
		{
			// read username and room from the buffer
			int usernameLen = buffer.readIntBE();
			std::string username = buffer.readStringBE(usernameLen);

			int roomNameLen = buffer.readIntBE();
			std::string roomName = buffer.readStringBE(roomNameLen);

			std::string resultMsgs = username + " left " + roomName;
			messageHistory.push_back(resultMsgs.c_str());

			break;
		}

		case 6: // server informs other users when a new user joins a room
		{
			// read username and room from the buffer
			int usernameLen = buffer.readIntBE();
			std::string username = buffer.readStringBE(usernameLen);

			int roomNameLen = buffer.readIntBE();
			std::string roomName = buffer.readStringBE(roomNameLen);

			// add join message to the message history
			std::string resultMsgs = username + " joined " + roomName;
			messageHistory.push_back(resultMsgs.c_str());

			break;
		}

		case 7: // server informs the client of successfull authentication
		{
			// read username and room from the buffer
			int user_id = buffer.readIntBE();
		
			int createdAtLen = buffer.readIntBE();
			std::string createDate = buffer.readStringBE(createdAtLen);

			// add join message to the message history
			std::string resultMsgs = std::string("User Id " + std::to_string(user_id) + " logged in. Account created on: " + createDate);
			messageHistory.push_back(resultMsgs.c_str());

			detachCIN();

			break;
		}

		case 8:
		{
			int reason = buffer.readIntBE();

			switch (reason) {
			case 1:
				promptMenu(20, 120, "Invalid Credentials. Please try again!");

				//invalid credentials
				break;

			case 2:
				// server error

				promptMenu(20, 120, "Internal Server Error!");

				break;
			}

			break;
		}

		case 9:
		{
			int user_id = buffer.readIntBE();

			promptMenu(20, 120, "Register successfull. Your user id is:" + std::to_string(user_id));

			break;
		}

		case 10:
		{
			int reason = buffer.readIntBE();

			switch (reason) {
			case 1:
				promptMenu(20, 120, "Register failed: Account already exists.");
				break;

			case 2:
				promptMenu(20, 120, "Register failed: Password must be between 4-255 characters. Pick another password you putz.");
				break;

			case 3:
				promptMenu(20, 120, "Register failed: Internal Server Error. Does not compute.");
				break;
			}

			break;
		}

		}

		//get rid of the data we have used
		buffer.flushBuffer();

		if (buffer.getLength() >= 8) {
			// recursive check until entire message packet is recieved
			this->handleMessage(buffer, socketId);
		}

		cClientMessageHandler::getInstance()->updateUI();
	}
}

/*
*	updateUI()
*	This function handles updating the clients console after each input.
*	Accepts: None
*	Returns: None
*/
void cClientMessageHandler::updateUI() {

	cConsole console;
	CONSOLE_SCREEN_BUFFER_INFO consoleDetails = console.get_console_details();

	//get the window details
	int columns = consoleDetails.srWindow.Right - consoleDetails.srWindow.Left + 1;
	int rows = consoleDetails.srWindow.Bottom - consoleDetails.srWindow.Top + 1;

	std::stringstream ss;
	for (int i = 0; i < columns; ++i) {
		ss << '*';
	}
	ss << std::endl;
	
	console.setCurrentRow(2);
	console.log(ss.str().c_str());
	console.setCurrentRow(rows - 1);
	console.log(ss.str().c_str());
	console.setCurrentRow(3);

	// handle how many messages are on the screen at once 
	if (this->messageHistory.size() > 25) {
		size_t offsetToStartAt = this->messageHistory.size() - 25;

		std::vector<std::string> reducedVec(this->messageHistory.begin() + offsetToStartAt, this->messageHistory.end());
		this->messageHistory = reducedVec;
	}

	// print message history to client console
	for (int i = 0; i != this->messageHistory.size(); ++i) {
		console.log("%s", this->messageHistory[i].c_str());
	}

	// reset client input
	console.setCurrentRow(rows);
	console.clearRow(rows);
	console.log(">>");
	console.gotoxy(3, rows);
	

}