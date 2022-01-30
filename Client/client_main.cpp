
#include "cTCPClient.h"
#include "../include/cMessageProtocolHelper.h"

#include <atomic>
#include <thread>
#include <iostream>
#include "cClientMessageHandler.h"
#include <limits>
#undef max

cConsole console;				// console input helper class
cTCPClient client;				// client socket
cMessageProtocolHelper helper; // commands to be sent to the server
int currentRow = 20;
bool isFirstRun = true;

char username[255] = { 0 };
char password[255] = { 0 };

void ReadCin(std::atomic<bool>& run);

std::atomic<bool> run(true);
void detachCIN() {
	// read input from the user (user logged in and in chat now do the chat stuff)
	std::thread cinThread(ReadCin, std::ref(run));

	cinThread.detach();
}

void promptLogin(int rows, int columns) {
	
	//loop until a username is passed in
	do {
		system("cls");
		// Prompt the user to enter a name 
		console.setCurrentRow(rows / 2 - 1);
		console.gotoxy(columns / 2 - 9, rows / 2 - 5);
		std::cout << "Enter a Username:\n";
		console.gotoxy(columns / 2 - 9, rows / 2 - 4);
		std::cin.getline(username, 255);
		console.clearRow(rows / 2 - 5);
		console.clearRow(rows / 2 - 4);

		if (username[0] != '\0') {
			break;
		}
	} while (true);

	do {
		system("cls");
		// Prompt the user to enter a password 
		console.setCurrentRow(rows / 2 - 1);
		console.gotoxy(columns / 2 - 9, rows / 2 - 5);
		std::cout << "Enter your password:\n";
		console.gotoxy(columns / 2 - 9, rows / 2 - 4);
		std::cin.getline(password, 255);
		console.clearRow(rows / 2 - 5);
		console.clearRow(rows / 2 - 4);

		if (password[0] != '\0') {
			break;
		}
	} while (true);

	//todo: authenticate or error msg
	// send the username/password to the server
	client.sendMessage(helper.createServerJoinMessage(username, password));

	system("cls");
}

void promptRegisterAccount(int rows, int columns) {

	//loop until a username is passed in
	do {
		system("cls");
		// Prompt the user to enter a name 
		console.setCurrentRow(rows / 2 - 1);
		console.gotoxy(columns / 2 - 9, rows / 2 - 5);
		std::cout << "Enter a Username:\n";
		console.gotoxy(columns / 2 - 9, rows / 2 - 4);
		std::cin.getline(username, 255);
		console.clearRow(rows / 2 - 5);
		console.clearRow(rows / 2 - 4);

		if (username[0] != '\0') {
			break;
		}
	} while (true);

	do {
		system("cls");
		// Prompt the user to enter a password 
		console.setCurrentRow(rows / 2 - 1);
		console.gotoxy(columns / 2 - 9, rows / 2 - 5);
		std::cout << "Enter your password:\n";
		console.gotoxy(columns / 2 - 9, rows / 2 - 4);
		std::cin.getline(password, 255);
		console.clearRow(rows / 2 - 5);
		console.clearRow(rows / 2 - 4);

		if (password[0] != '\0') {
			break;
		}
	} while (true);

	//todo: register or error msg
	client.sendMessage(helper.createRegisterMessage(username, password));

	system("cls");
}

void promptMenu(int rows, int columns, std::string errorMsg) {
	int selectedOption = 0;
	do {
		console.setCurrentRow(rows / 2 - 4);

		if (errorMsg != "") {
			console.log(errorMsg.c_str());
		}

		// Prompt the user to enter a name 
		console.setCurrentRow(rows / 2 - 1);
		console.gotoxy(columns / 2 - 9, rows / 2 - 5);
		console.log("Select An Option:\n");
		console.gotoxy(columns / 2 - 9, rows / 2 - 4);

		console.log("1. Login");
		console.log("2. Register Account");
		console.log("3. Quit");

		console.gotoxy(0, rows - 1);
		std::cout << ">> ";
		
		while (!(std::cin >> selectedOption)) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}

		if (selectedOption > 0 && selectedOption <= 3) {
			break;
		}

	} while (true);

	switch (selectedOption) {
	case 1:
		promptLogin(rows, columns);
		break;
	case 2:
		promptRegisterAccount(rows, columns);
		break;
	case 3:
		exit(0);
		break;
	}
}

void ReadCin(std::atomic<bool>& run)
{
	//get the window details
	CONSOLE_SCREEN_BUFFER_INFO consoleDetails = console.get_console_details();
	int columns = consoleDetails.srWindow.Right - consoleDetails.srWindow.Left + 1;
	int rows = consoleDetails.srWindow.Bottom - consoleDetails.srWindow.Top + 1;

	std::string buffer;

	while (run.load())
	{
		std::cin >> buffer;

		if (buffer == "/quit") {
			// user quit the chat, end input and send cleanup commands
			// to server
			run.store(false);

			client.closeSendSocket();
			client.cleanUp();

			exit(0);

		} else if (buffer == "/join") {
			// user joined a room, send join room command to server.
			std::string roomName;
			std::cin >> roomName;

			client.sendMessage(helper.createJoinRoomMessage(roomName));
		}

		else if (buffer == "/leave") {
			// user left the room, send leave command to server
			std::string roomName;
			std::cin >> roomName;

			client.sendMessage(helper.createLeaveRoomMessage(roomName));
		}

		else if (buffer == "/say") {

			std::string roomName;
			std::cin >> roomName;

			char message[255];
			std::cin.getline(message, 255);

			client.sendMessage(helper.createSendMessage(roomName, message));
		}
	}
}

int main(int argc, char **argv)
{
	
	cConsole console;

	console.lock_start_row();
	console.refresh();


	if (!client.initializeWinsock())
	{
		printf("WSAStartup failed\n");
		return 1;
	}

	// Step #2 Resolve the server address and port
	if (!client.resolveServerAddr())
	{
		printf("getaddrinfo failed.\n");
		WSACleanup();
		return 1;
	}

	if (!client.connectToServer()) {
		printf("cannot connect :( .\n");
		WSACleanup();
		return 1;
	}

	// Window formatting
	CONSOLE_SCREEN_BUFFER_INFO consoleDetails = console.get_console_details();
	int columns = consoleDetails.srWindow.Right - consoleDetails.srWindow.Left + 1;
	int rows = consoleDetails.srWindow.Bottom - consoleDetails.srWindow.Top + 1;

	promptMenu(rows,columns, "");

	// Receive until the peer closes the connection
	client.startListen();


	return 0;
}