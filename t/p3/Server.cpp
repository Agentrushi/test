#include <iostream>
#include<vector>
#include<string>
#include<winsock.h>
#pragma comment(lib,"wsock32.lib")
#define SERVER_PORT 110
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
	for (int i = 0; i < len; i++)
		cout << message[i];
	cout << endl;
}
bool startWith(const string& a, string b)
{
	bool pom = true;
	for (unsigned int i = 0; i < b.length(); i++)
		if (a[i] != b[i])
			pom = false;
	return pom;

}
int main()
{
	try
	{
		int velicinaPoslate = 0;
		string ok = "+OK";
		const int brojporuka = 4;
		const char* niz[brojporuka];

		niz[0] = "Ovo je poruka sa rednim brojem 1 koju je zahtevao klijent.";
		niz[1] = "Ovo je poruka sa rednim brojem 2 koju je zahtevao klijent.";
		niz[2] = "Ovo je poruka sa rednim brojem 3 koju je zahtevao klijent.";
		niz[3] = "Ovo je poruka sa rednim brojem 4 koju je zahtevao klijent.";

		long ukupnaVelicinaPoruka = 0;
		for (int i = 0; i < brojporuka; i++){
			ukupnaVelicinaPoruka += sizeof(niz[i]);
		}

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

			string potvrda1 = "+OK POP3 server ready";
			velicinaPoslate = send(clientSocket, potvrda1.c_str(), potvrda1.size(), 0);
			if (velicinaPoslate != potvrda1.size())
				ExitWithError("Poruka lose poslata klijentu.");

			vector<char> buff1(BUF_SIZE);
			int velicinaPoruke;
			//prvo mi stize Username
			if((velicinaPoruke = recv(clientSocket, buff1.data(), buff1.size(), 0)) < 0)
				ExitWithError("Poruka je lose stigla.");

			printMessage(buff1.data(), velicinaPoruke);

			velicinaPoslate = send(clientSocket, ok.c_str(), ok.size(), 0);
			if (velicinaPoslate != ok.size())
				ExitWithError("Poruka lose poslata klijentu.");

			//pa sada mi stize password
			if((velicinaPoruke = recv(clientSocket, buff1.data(), buff1.size(), 0)) < 0)
				ExitWithError("Poruka je lose stigla.");

			string potvrda2 = "+OK user successfully logged on";
			printMessage(buff1.data(), velicinaPoruke);
			velicinaPoslate = send(clientSocket, potvrda2.c_str(), potvrda2.size(), 0);
			if (velicinaPoslate != potvrda2.size())
				ExitWithError("Poruka lose poslata klijentu.");

			//Stizu komande
			while (1){
				
				if (velicinaPoruke = recv(clientSocket, buff1.data(), buff1.size(), 0) < 0)
					ExitWithError("Poruka je lose stigla. 124");

				if (startWith(buff1.data(), "list")){
					string brPoruka = to_string(brojporuka);
					string velicina = to_string(ukupnaVelicinaPoruka);
					string porukaList = "+OK " + brPoruka + " messages (" + velicina + " octets)" + "\n";

					for (int i = 0; i < brojporuka; i++)
						porukaList += to_string(i) + " " + to_string(sizeof(niz[i])) + "\n";

					porukaList += ".";
					velicinaPoslate = send(clientSocket, porukaList.c_str(), porukaList.size(), 0);
					if (velicinaPoslate != porukaList.size())
						ExitWithError("Poruka lose poslata klijentu.");
				}
				else if (startWith(buff1.data(), "retr")){
					char broj = buff1.data()[5];

					int velicina = sizeof(niz[broj]);

					string porukaRetr = "+OK " + to_string(velicina) + " octets" + "\n";
					
					porukaRetr += "blah..blah";

					porukaRetr += "\n";
					porukaRetr += ".";

					velicinaPoslate = send(clientSocket, porukaRetr.c_str(), porukaRetr.size(), 0);
					if (velicinaPoslate != porukaRetr.size())
						ExitWithError("Poruka lose poslata klijentu.");

				}
				else if (startWith(buff1.data(), "dele")){
					string porukaDele = "+OK message deleted";
					velicinaPoslate = send(clientSocket, porukaDele.c_str(), porukaDele.size(), 0);
					if (velicinaPoslate != porukaDele.size())
						ExitWithError("Poruka lose poslata klijentu.");
				}
				else if (startWith(buff1.data(), "quit")){
					string porukaDele = "+OK POP3 server closing connection";
					velicinaPoslate = send(clientSocket, porukaDele.c_str(), porukaDele.size(), 0);
					if (velicinaPoslate != porukaDele.size())
						ExitWithError("Poruka lose poslata klijentu.");
					break;
				}
			}
		}
	}
	catch (string s){
		cout << s << endl;
	}
}