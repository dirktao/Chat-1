#include "UserInterface.hpp"

UserInterface::UserInterface() {
	setlocale(LC_ALL, "");
	window = initscr();
	raw();
	keypad(window, 1);
	noecho();
}

UserInterface::~UserInterface() {
	userInput->detach();
	delete userInput;
	endwin();
}

void UserInterface::UpdateLog(Log *log) {
	int winX, winY;
	getmaxyx(window, winY, winX);

	int cursorX, cursorY;
	getyx(window, cursorY, cursorX);

	std::vector<std::string> visibleLines = log->GetLines(winY - 2, winX, 0);

	for(size_t i = 0; i < visibleLines.size(); ++i) {
		mvaddstr(winY - 2 - i, 0, visibleLines[i].c_str());
		clrtoeol();
	}

	move(cursorY, cursorX);
	refresh();
}

void UserInterface::GetUserInput(Server *server) {
	userInput = new std::thread(HandleInput, server, this->window);
}

bool UserInterface::UserInputRunning() {
	return userInput->joinable();
}

void UserInterface::HandleInput(Server *server, WINDOW *window) {
	std::string message("");
	int cursor = 0;

	int winX, winY;
	getmaxyx(window, winY, winX);
	move(winY - 1, cursor);

	bool active = 1;

	while(active) {
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

		if(action == "disconnect")
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