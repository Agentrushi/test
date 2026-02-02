#include <iostream>
#include <string>
#include <vector>
#include <winsock.h>
#pragma comment(lib, "wsock32.lib")
#define SERVER_PORT 80
#define BUF_SIZE 1024

void ExitWithError(const std::string& message) {
	std::cout << message << "Error code:" << WSAGetLastError() << std::endl;
	WSACleanup();
	exit(1);
}
void printMessage(const std::string& message, int len) {
	std::cout << "Primljena poruka:";
	for (auto i = 0; i < len; std::cout << message[i++]);
	std::cout << std::endl;
}

void HandleError(const char* msg) {
	std::cerr << msg << std::endl;
}

int main() {
	// Inicijalizacija winsoketa
	WSAData wsa;
	SOCKET listensock;
	if (WSAStartup(0x0202, &wsa) != 0)
		ExitWithError("Startup failed.");
	// Kreiranje listen soketa
	if ((listensock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		ExitWithError("Listening socket not created");

	// Dodela porta listen soketu, bind
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(SERVER_PORT);
	if (bind(listensock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
		ExitWithError("bind() failed!");
	//Osluskivanje
	if ((listen(listensock, 3)) < 0)
		ExitWithError("listen() failed!");

	while (true) {
		sockaddr_in client;
		int cLen = sizeof(struct sockaddr_in);
		SOCKET clientSock;
		if ((clientSock = accept(listensock, (struct sockaddr*)&client, &cLen)) < 0)
			HandleError("accept() failed!");
		// Primanje poruke od klijenta
		std::vector<char> echoBuf(BUF_SIZE);
		int recvMsgSize = recv(clientSock, echoBuf.data(), echoBuf.size(), 0);
		if (recvMsgSize < 0)
			HandleError("recv() failed");
		printMessage(echoBuf.data(), recvMsgSize);
		while (recvMsgSize > 0)
		{
			std::string msg = "<html><head><title>Strana</title></head><body>nebitno</body></html>";
			std::string res = "HTTP/1.1 200 OK\r\nConnection: close\r\nDate: Thu, 26 Apr 2005 07:00:15 GMT\r\nServer: Apache / 1.3.0 (Unix)\r\nLast-Modified: Mon, 22 Jun 1998 09:23:24 GMT\r\nContent-Length: " + std::to_string(msg.length()) + "\r\nContent-Type: text/html\r\n\r\n" + msg;
			if (send(clientSock, res.c_str(), res.length(), 0) != res.length())
				HandleError("send() failed");
			if ((recvMsgSize = recv(clientSock, echoBuf.data(), echoBuf.size(), 0)) < 0)
				HandleError("recv() failed");
			printMessage(echoBuf.data(), recvMsgSize);
		} // while (recvMsgSize > 0)
		closesocket(clientSock);
	} //while (true)
	closesocket(listensock);
	WSACleanup();
	return 0;
}