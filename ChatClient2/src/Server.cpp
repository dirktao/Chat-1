#include "Server.hpp"

Server::Server(Log *log, const char *host, int port) {
	this->log = log;
	this->host = host;
	this->port = port;
	this->sock = -1;
}

Server::~Server() {
	if(sock != -1)
		this->Disconnect();
}

void Server::Connect() {
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		sock = -1;
		log->Add("30");
		return;
	}

	if((hostInfo = gethostbyname(host)) == NULL) {
		sock = -1;
		log->Add("31");
		return;
	}

	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	memcpy((char *) &serverAddress.sin_addr.s_addr, (char *) hostInfo->h_addr, hostInfo->h_length);
	serverAddress.sin_port = htons(port);

	if(connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
		sock = -1;
		log->Add("32");
		return;
	}

	if(fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK) < 0) {
		sock = -1;
		log->Add("33");
		return;
	}
}

int Server::GetFD() {
	return sock;
}

bool Server::IsAlive() {
	return (sock != -1 ? 1 : 0);
}

bool Server::Send(std::string message) {
	message += "\n";

	ssize_t sent;
	do {
		sent = write(sock, message.data(), message.size());
		if(sent == -1) {
			this->Disconnect();
			return 0;
		}
		message.erase(0, sent);
	}
	while(message.size() > 0);
	return 1;
}

void Server::Read() {
	int isAlive = 0;
	ioctl(sock, FIONREAD, &isAlive);
	if(isAlive == 0) {
		this->Disconnect();
		return;
	}

	char charBuffer[4096];
	ssize_t received = 0;

	while((received = recv(sock, charBuffer, sizeof(charBuffer), 0)) > 0) {
		buffer += std::string(charBuffer, received);
		memset(charBuffer, 0, 4096);
	}

	for(size_t messageFind = buffer.find("\n"); messageFind != std::string::npos; messageFind = buffer.find("\n")) {
		std::string message = buffer.substr(0, messageFind);
		buffer.erase(0, messageFind + 1);
		log->Add(message);
	}
}

void Server::Disconnect() {
	close(sock);
	if(sock == -1)
		log->Add("36");
	else {
		sock = -1;
		log->Add("35");
	}
}