#pragma once
#include "utils/utils.h"


class cConsole {
public:
	cConsole();
	virtual ~cConsole();
	void log(const char* message, ...);
	void lock_start_row();
	void reset_start_row();
	void refresh();
	void setCurrentRow(int row) {
		this->mcurrent_row = row;
	}
	void gotoxy(int x, int y);

	CONSOLE_SCREEN_BUFFER_INFO get_console_details() {
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		
		return csbi;
	}

	void clearRow(int row) {
		int temp = this->mcurrent_row;
		this->setCurrentRow(row);
		this->log("                                                                                                                           ");

		this->mcurrent_row = temp;
	}

private:
	HANDLE mh_out;
	CONSOLE_CURSOR_INFO mcon_cur_inf;

	int mcurrent_row;
	int mstart_row;

	
};