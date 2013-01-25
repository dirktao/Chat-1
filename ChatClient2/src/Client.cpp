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
	while(ui->GetInput())
		ui->UpdateLog();
	server->Disconnect();
}