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
bool startWith(const string& a, string b)
{
	bool pom = true;
	for (int i = 0; i < b.length(); i++)
		if (a[i] != b[i])
			pom = false;
	return pom;

}

void rastavi(string s, int vel, string& s1, string& s2, string& s3) {
	s1 += s.substr(0, 3);
	int index = 3;
	int i = 4;
	while (s[i] != ' ') i++;
	index++;

	s2 += s.substr(index, i - index);
	index = i;
	while (s[i] != '\n') i++;
	index++;
	s3 += s.substr(index, i - index);
	cout << s1 << ":" << s2 << ":" << s3 << endl;
}

void rastavi(string s, int vel, string& s1) {
	int index = 0;
	while (s[index] != ':') index++;
	index++;
	s1 += s.substr(index, s.length() - index);
}
int main()
{
	try
	{
		int velicinaPoslate = 0;
		string ok = "HTTP/1.0 200 OK";
		string notFound = "HTTP/1.0 404 Not Found";
		string notMod = "HTTP/1.0 304 Not Modified";
		string  version = "HTTP/1.0 505 HTTP Version Not Supported";
		string http = "HTTP/1.0";


		string slika = "img.jpg";
		string lastMod = "Mon, 22 Jun 1998 09:23:24";
		string part2 = "";

		WSAData wsa;
		if (WSAStartup(0x0202, &wsa) != 0)
			ExitWithError("WSA failed.");
		SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
		if (listenSock == INVALID_SOCKET)
			ExitWithError("Listen socket failed.");

		//dodela porta serveru
		sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_port = htons(SERVER_PORT);
		server.sin_addr.s_addr = INADDR_ANY;

		int status = bind(listenSock, (struct sockaddr*)&server, sizeof(server));
		if (status == -1)
			ExitWithError("Bind() failed.");
		while (1) {
			//vrsim osluskivanje
			int status1 = listen(listenSock, 2);
			if (status1 == -1)
				ExitWithError("Listen() failed.");

			sockaddr_in client;
			int cLen = sizeof(client);
			SOCKET clientSocket = accept(listenSock, (struct sockaddr*)&client, &cLen);
			if (clientSocket == -1)
				ExitWithError("Wrong client socket.");
			else
			{
				string potvrda1 = "OK HTTP/1.0 server ready";
				velicinaPoslate = send(clientSocket, potvrda1.c_str(), potvrda1.size(), 0);
				if (velicinaPoslate != potvrda1.size())
					ExitWithError("Poruka lose poslata klijentu.");
			}
			vector<char> buff1(BUF_SIZE);

			int velicinaPoruke = recv(clientSocket, buff1.data(), buff1.size(), 0);
			if (velicinaPoruke < 0)
			{
				break;
			}
			else
			{
				printMessage(buff1.data(), velicinaPoruke);

				string kom = "";
				string sl = "";
				string httpV = "";
				rastavi(buff1.data(), velicinaPoruke, kom, sl, httpV);
				rastavi(buff1.data(), velicinaPoruke, part2);

				if (kom.compare("GET") == 0) {
					if (sl.compare(slika) == 0)
					{
						if (httpV.compare(http) == 0)
						{
							if (part2.compare(lastMod) < 0) {
								cout << ok.c_str() << " " << ok.size() << endl;
								velicinaPoslate = send(clientSocket, ok.c_str(), ok.size(), 0);
								if (velicinaPoslate != ok.size())
									break;
								cout << velicinaPoslate << endl;
							}
							else {
								cout << notMod.c_str() << " " << notMod.size() << endl;
								velicinaPoslate = send(clientSocket, notMod.c_str(), notMod.size(), 0);
								if (velicinaPoslate != notMod.size())
									break;
								cout << velicinaPoslate << endl;
							}
						}
						else {
							velicinaPoslate = send(clientSocket, version.c_str(), version.size(), 0);
							if (velicinaPoslate != version.length())
								break;
						}
					}
					else
					{
						velicinaPoslate = send(clientSocket, notFound.c_str(), notFound.size(), 0);
						if (velicinaPoslate != notFound.size())
							break;
					}
				}
				else
				{
					velicinaPoslate = send(clientSocket, notFound.c_str(), notFound.size(), 0);
					if (velicinaPoslate != notFound.size())
						break;
				}

			}
		}

	}
	catch (string s)
	{
		cout << s << endl;
	}



}