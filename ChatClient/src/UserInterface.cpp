#include "UserInterface.hpp"

UserInterface::UserInterface(Log *log, Server *server) {
	setlocale(LC_ALL, "");
	window = initscr();
	raw();
	keypad(window, 1);
	noecho();

	this->log = log;
	this->server = server;
	logSkip = 0;
	this->userInputRunning = 1;
}

UserInterface::~UserInterface() {
	userInput->detach();
	delete userInput;
	endwin();
}

void UserInterface::UpdateLog() {
	int winX, winY;
	getmaxyx(window, winY, winX);

	int cursorX, cursorY;
	getyx(window, cursorY, cursorX);

	std::vector<std::string> visibleLines = log->GetLines(winY - 2, winX, logSkip);

	for(size_t i = 0; i < visibleLines.size(); ++i) {
		mvaddstr(winY - 2 - i, 0, visibleLines[i].c_str());
		clrtoeol();
	}

	move(cursorY, cursorX);
	refresh();
}

void UserInterface::GetUserInput() {
	userInput = new std::thread(HandleInput, server, window, &logSkip, &userInputRunning);
}

void UserInterface::HandleInput(Server *server, WINDOW *window, int *logSkip, bool *userInputRunning) {
	std::string message("");
	int cursor = 0;

	int winX, winY;
	getmaxyx(window, winY, winX);
	move(winY - 1, cursor);

	bool active = 1;

	while(active && *userInputRunning) {
		int keyPress = getch();

		//if(keyPress >= 32 && keyPress <= 254 && keyPress != 127) {
		if(keyPress >= 32 && keyPress <= 126) {
			message.insert(cursor, std::string(1, (char) keyPress));
			++cursor;
		}
		else {
			switch(keyPress) {
				case 10: // Enter
					if(message.size() > 0) {
						active = HandleMessage(server, message);
						cursor = 0;
						message.clear();
					}
					break;
				case 260: // Left arrow
					if(cursor > 0)
						--cursor;
					break;
				case 261: // Right arrow
					if(cursor < (int) message.size())
						++cursor;
					break;
				case 262: // Home
					cursor = 0;
					break;
				case 263: // Backspace
					if(cursor > 0)
						message.erase(--cursor, 1);
					break;
				case 338: // Page down
					*logSkip -= 1;
					break;
				case 339: // Page up
					*logSkip += 1;
					break;
				case 360: // End
					cursor = message.size();
					break;
			}
		}

		getmaxyx(window, winY, winX);
		mvaddstr(winY - 1, 0, message.c_str());
		clrtoeol();
		move(winY - 1, cursor);
		refresh();
	}

	*userInputRunning = 0;
}

bool UserInterface::HandleMessage(Server *server, std::string message) {
	if(message.substr(0, 1) != "/") {
		server->Send("3 " + message);
	}
	else {
		std::string action;
		message.erase(0, 1);

		if(message.find(" ") == std::string::npos) {
			action = message;
			message.clear();
		}
		else {
			action = message.substr(0, message.find(" "));
			message.erase(0, action.size());
		}

		if(action == "connect") {
			if(server->IsAlive())
				server->Disconnect();
			server->Connect();
		}
		else if(action == "disconnect")
			server->Send("2" + message);
		else if(action == "quit") {
			server->Send("2" + message);
			return 0;
		}
		else if(action == "names")
			server->Send("4");
		else if(action == "nick")
			server->Send("5" + message);
		else if(action == "who")
			server->Send("6" + message);
		else if(action == "msg")
			server->Send("7" + message);
		else if(action == "ops")
			server->Send("10");
		else if(action == "op")
			server->Send("11" + message);
		else if(action == "deop")
			server->Send("12" + message);
		else if(action == "bans")
			server->Send("13");
		else if(action == "ban")
			server->Send("14" + message);
		else if(action == "unban")
			server->Send("15" + message);
		else
			server->Send(action);
	}

	return 1;
}