#ifndef _USERINTERFACE_HPP
#define _USERINTERFACE_HPP

#include <locale.h>
#include <thread>
#include <ncurses.h>

#include "Log.hpp"
#include "Server.hpp"

class UserInterface {
	WINDOW *window;
	std::thread *userInput;
	public:
		UserInterface();
		~UserInterface();
		void UpdateLog(Log *log);
		void GetUserInput(Server *server);
		bool UserInputRunning();
	private:
		static void HandleInput(Server *server, WINDOW *window);
		static bool HandleMessage(Server *server, std::string message);
};

#endif