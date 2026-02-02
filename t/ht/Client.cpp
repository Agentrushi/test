#include <iostream>
#include<vector>
#include<string>
#include<winsock.h>
#pragma comment(lib,"wsock32.lib")
#define SERVER_PORT 80
#define BUF_SIZE 1024
using namespace std;
void ExitWithError(const string& message)
{
	cout << message << "Error code:" << WSAGetLastError() << endl;
	WSACleanup();
	exit(1);
}
void printMessage(const string& message, int len)
{
	cout << "Primljena poruka:" << endl;
	for (auto i = 0; i < len; i++)
		cout << message[i];
	cout << endl;
}
bool isValid(const string& a, string b)
{
	bool pom = true;
	for (int i = 0; i < b.length() ; i++)
		if (a[i] != b[i])
			pom = false;
	return pom;

}

bool startWith(const string& a, string b)
{
	bool pom = true;
	for (int i = 0; i < 2; i++)
		if (a[i] != b[i])
			pom = false;
	return pom;

}

int main()
{
	WSAData wsa;
	const char* serverAddr = "127.0.0.1";
	if (WSAStartup(0x0202, &wsa) != 0)
		throw("WSA failed.");
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket < 0)
		throw ("clientSocked failed()");

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);
	server.sin_addr.s_addr = inet_addr(serverAddr);

	int statusCon = connect(clientSocket, (struct sockaddr*)&server, sizeof(server));
	if (statusCon == SOCKET_ERROR)
		throw("Bad connection.");
	
	int velicinaPoruke = 0;
	vector<char> buff(BUF_SIZE);
	if ((velicinaPoruke = recv(clientSocket, buff.data(), buff.size(), 0)) < 0)
		ExitWithError("Recv() failed.");
	if (!startWith(buff.data(), "OK"))
		ExitWithError("Recv() failed.");
	printMessage(buff.data(), velicinaPoruke);

	cout << "Unesite podataka koji trazite od servera:" << endl;
	string fajl;
	getline(cin, fajl);
	string poruka = "GET " + fajl;
	poruka += " HTTP/1.0";
	poruka += "\n";
	poruka += "If-modified-since:Mon, 22 Jun 1998 09:23:24";
	poruka += "\0";

	cout << "Klijent prosledjuje serveru " << poruka << endl;

	if (send(clientSocket, poruka.c_str(), poruka.length(), 0) != poruka.length())
		ExitWithError("Poruka lose poslata.");
	
	/*if ((velicinaPoruke = recv(clientSocket, buff.data(), buff.size(), 0)) < 0)
		ExitWithError("Recv() failed.");*/
	string p;
	velicinaPoruke = recv(clientSocket,buff.data(), buff.size(), 0);
	printMessage(buff.data(), velicinaPoruke);

	closesocket(clientSocket);
	WSACleanup();
}