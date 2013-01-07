#include "Client.hpp"

Client::Client(const char *host, int port) {
	log = new Log(0);
	server = new Server(log, host, port);
	ui = new UserInterface(log, server);
}

Client::~Client() {
	delete server;
	delete ui;
	delete log;
}

void Client::Run() {
	server->Connect();
	ui->GetUserInput();
	while(ui->userInputRunning) {
		if(server->IsAlive())
			server->Read();
		else
			usleep(10000);
		ui->UpdateLog();
	}
}