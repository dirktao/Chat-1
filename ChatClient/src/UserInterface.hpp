#ifndef _USERINTERFACE_HPP
#define _USERINTERFACE_HPP

#include <locale.h>
#include <thread>
#include <ncurses.h>

#include "Log.hpp"
#include "Server.hpp"

class UserInterface {
	WINDOW *window;
	Log *log;
	int logSkip;
	Server *server;
	std::thread *userInput;
	public:
		bool userInputRunning;
		UserInterface(Log *log, Server *server);
		~UserInterface();
		void UpdateLog();
		void GetUserInput();
	private:
		static void HandleInput(Log *log, Server *server, WINDOW *window, int *logSkip, bool *userInputRunning);
		static bool HandleMessage(Log *log, Server *server, std::string message);
};

#endif