#include <iostream>
#include <string>
#include <winsock.h>
#include <vector>
#pragma comment(lib, "wsock32.lib")
#define SERVER_PORT 80
#define BUF_SIZE 2048

void ExitWithError(const std::string& message) {
	std::cout << message << "Error code:" << WSAGetLastError() << std::endl;
	WSACleanup();
	exit(1);
}
void printMessage(const std::string& message, int len) {
	//std::cout << message << std::endl;
	for (auto i = 0; i < len; std::cout << message[i++]);
	std::cout << std::endl;
}

int main() {
	WSAData wsa;
	SOCKET clientSocket;
	if (WSAStartup(0x0202, &wsa) != 0)
		ExitWithError("Startup failed.");
	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		ExitWithError("Listening socket not created");
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("160.99.14.201");
	server.sin_port = htons(SERVER_PORT);
	if (connect(clientSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
		ExitWithError("connect() failed!");
	while (true) {
		std::string getRequest = "GET /downloads/akreditacija-2019/oas/rii/3OER3O01-diskretna-matematika.pdf HTTP/1.1\r\nConection: close\r\nUser-agent: Mozilla/5.0\r\nHost: www.elfak.ni.ac.rs\r\nIf-modified-since: Wed, 12 Dec 2020 13:31:03 GMT\r\nAccept: \r\nAccept-language: en\r\n\r\n";
		if (send(clientSocket, getRequest.c_str(), getRequest.length(), 0) != getRequest.length())
			ExitWithError("send() failed");
		int recvMsgSize = 0;
		std::vector<char> buff(BUF_SIZE);
		do {
			if ((recvMsgSize = recv(clientSocket, buff.data(), buff.size(), 0)) < 0)
				ExitWithError("recv() failed");
			printMessage(buff.data(), recvMsgSize);
		} while (recvMsgSize == BUF_SIZE);
		int x;
		std::cin >> x;
	}
	closesocket(clientSocket);
	WSACleanup();
}