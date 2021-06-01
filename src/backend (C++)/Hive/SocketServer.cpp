#include "SocketServer.h"

SocketServer::SocketServer() {
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != NO_ERROR)
		std::cout << "Initialization error.\n";

	mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mainSocket == INVALID_SOCKET)
	{
		std::cout << "Error creating socket: %ld\n", WSAGetLastError();
		WSACleanup();
	}

	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(4000);

	if (bind(mainSocket, (SOCKADDR*)& service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("bind() failed.\n");
		closesocket(mainSocket);
	}
}

void SocketServer::Connect() {
	if (listen(mainSocket, 1) == SOCKET_ERROR)
		printf("Error listening on socket.\n");

	SOCKET acceptSocket = SOCKET_ERROR;
	printf("Waiting for a client to connect...\n");

	while (acceptSocket == SOCKET_ERROR)
	{
		acceptSocket = accept(mainSocket, NULL, NULL);
	}

	printf("Client connected.\n");
	mainSocket = acceptSocket;
}

std::string SocketServer::Recv() {
	char buffer[1024];
	int bytesReceived = recv(mainSocket, buffer, 1024, 0);
	return std::string(buffer).substr(0, bytesReceived);
}

void SocketServer::Send(std::string data) {
	char* buffer = new char[data.size() + 1];
	strcpy(buffer, data.c_str());
	send(mainSocket, buffer, strlen(buffer), 0);
	delete[] buffer;
}