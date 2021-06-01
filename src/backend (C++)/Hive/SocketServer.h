#pragma once

#include "headers.h"
class SocketServer {
	WSADATA wsaData;
	SOCKET mainSocket;
	sockaddr_in service;
public:
	SocketServer();
	void Connect();
	std::string Recv();
	void Send(std::string str);
};