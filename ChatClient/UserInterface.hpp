#ifndef _USERINTERFACE_HPP
#define _USERINTERFACE_HPP

#include <iostream>
#include <locale.h>
#include <thread>
#include <ncurses.h>

#include "Log.hpp"
#include "Server.hpp"

class UserInterface {
	WINDOW *window;
	std::thread *inputHandler;
	public:
		UserInterface();
		~UserInterface();
		void UpdateLog(Log *log);
		void RunHandleInput(Server *server);
	private:
		static void HandleInput(Server *server, WINDOW *window);
		static void HandleInputMessage(Server *server, std::string message);
};

#endif