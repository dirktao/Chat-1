#ifndef _CLIENT_HPP
#define _CLIENT_HPP

#include <unistd.h>

#include "Log.hpp"
#include "Server.hpp"
#include "UserInterface.hpp"

class Client {
	Log *log;
	UserInterface *ui;
	Server *server;
	public:
		Client(const char *host, int port);
		~Client();
		void Run();
};

#endif