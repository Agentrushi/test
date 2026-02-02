#define _CRT_SECURE_NO_WARNINGS
#include <winsock.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#pragma comment(lib, "wsock32.lib")


using namespace std;

int main()
{
	WSAData wsa;
	WSAStartup(0x0202, &wsa);

	SOCKET sock;
	sockaddr_in server;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
	{
		closesocket(sock);
		WSACleanup();
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(80);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (sockaddr*)&server, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		closesocket(sock);
		WSACleanup();
		exit(1);
	}

	if (listen(sock, 100) == SOCKET_ERROR)
	{
		closesocket(sock);
		WSACleanup();
		exit(1);
	}

	while (true)
	{
		SOCKET comSock;
		sockaddr_in client;

		int addr_len = sizeof(sockaddr);
		if ((comSock = accept(sock, (sockaddr*)&client, &addr_len)) == SOCKET_ERROR)
		{
			closesocket(sock);
			WSACleanup();
			exit(1);
		}

		char sendBuf[200000];
		char recvBuf[1024];

		ZeroMemory(sendBuf, 200000);// ispu
		ZeroMemory(recvBuf, 1024);

		int bytes_send;
		int bytes_recv;

		bytes_recv = recv(comSock, recvBuf, 1024, 0);
		if (bytes_recv < 0) //broj bajta koji primimo, verovatn o greska ako je manje od 0
		{
			closesocket(sock);
			WSACleanup();
			exit(1);
		}

		printf("%s\n", recvBuf);

		char fileName[25];
		int j = 0;
		char* t = strstr(recvBuf, "/");
		t++;
		while (*t != ' ')
			fileName[j++] = *t++;
		fileName[j] = '\0';

		char path[100];
		ZeroMemory(path, 100);
		strcat(path, "C:\\Users\STRONG\Desktop\Laboratorijska vezba 5\LAB V\SERVER KOJI VRACA TRAZENU SLIKU\\");
		strcat(path, fileName);

		char temp[10000];
		char body[65536];
		char body_len[100];
		ZeroMemory(temp, 10000);
		ZeroMemory(body, 65536);

		ifstream myFile(path, ios::binary | ios::in);

		long len;
		char* buf = NULL;
		FILE* fp = NULL;
		fp = fopen(path, "rb");


		if (myFile)
		{
			if (fseek(fp, 0, SEEK_END) != 0)
			{
				fclose(fp);
			}
			len = ftell(fp);
			rewind(fp);
			buf = (char*)malloc(sizeof(char) * len);
			if (!buf)
			{
				fclose(fp);
			}


			strcat(sendBuf, "HTTP/1.1 200 OK\r\n");
			_itoa(len, body_len, 10);
			//strcat(sendBuf, "Content-Type: image/jpg\r\n");
			strcat(sendBuf, "Content-Length: ");
			strcat(sendBuf, body_len);
			strcat(sendBuf, "\r\n");
			strcat(sendBuf, "Content-Type: image/png\r\n\r\n");
			if (!fread(sendBuf, len, 1, fp))
			{
				fclose(fp);
			}
			fclose(fp);
			//	strcat(sendBuf, body);

			bytes_send = send(comSock, sendBuf, strlen(buf), 0);
			if (bytes_send < 0)
			{
				closesocket(sock);
				WSACleanup();
				exit(1);
			}

			closesocket(comSock); //ovo ne treba zaboraviti!
		}
		else
		{
			if (strstr(fileName, ".png") != NULL) //proverava ekstenziju zahtevanog fajla...
			{

				strcat(sendBuf, "HTTP/1.1 404 Not Found\nContent-Length: 26\nContent-Type: text/plain\n\n404 Error: File not Found!");
				bytes_send = send(comSock, sendBuf, strlen(sendBuf), 0);
				if (bytes_send < 0)
				{
					closesocket(sock);
					WSACleanup();
					exit(1);
				}
			}
			else
			{
				strcat(sendBuf, "HTTP/1.1 400 Bad Request\nContent-Length: 23\nContent-Type: text/plain\n\n400 Error: Bad request!");
				bytes_send = send(comSock, sendBuf, strlen(sendBuf), 0);
				if (bytes_send < 0)
				{
					closesocket(sock);
					WSACleanup();
					exit(1);
				}
			}

			closesocket(comSock); //ovo ne treba zaboraviti!
		}
	}

	closesocket(sock);
	WSACleanup();

	return 0;
}