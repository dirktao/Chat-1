#include <iostream>

#include "Client.hpp"

int main(int argc, char *argv[]) {
	if(argc < 3) {
		std::cout << "Usage: [ChatClient] [host] [port]" << std::endl;
		return 1;
	}

	Client *client = new Client(argv[1], atoi(argv[2]));
	client->Run();
	delete client;

	return 0;
}