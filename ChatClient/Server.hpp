#ifndef _SERVER_HPP
#define _SERVER_HPP

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "Log.hpp"

class Server {
	int sock;
	struct hostent *hostInfo;
	struct sockaddr_in serverAddress;
	fd_set fdSet;
	int fdMax;
	std::string buffer;
	public:
		const char *host;
		int port;
		Server(const char *host, int port);
		~Server();
		void Connect(Log *log);
		bool IsAlive();
		bool Send(std::string message);
		void Read(Log *log);
		void Disconnect();
};

#endif