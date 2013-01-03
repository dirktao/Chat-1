#include "Client.hpp"

Client::Client(const char *host, int port) {
	log = new Log(0);
	ui = new UserInterface();
	server = new Server(host, port);
}

Client::~Client() {
	delete server;
	delete ui;
	delete log;
}

void Client::Run() {
	server->Connect(log);
	ui->GetUserInput(server);
	while(server->IsAlive() && ui->UserInputRunning()) {
		server->Read(log);
		ui->UpdateLog(log);
	}
}