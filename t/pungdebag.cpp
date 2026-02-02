#include <iostream>
#include <winsock.h>
#include <fstream>
#include <sstream>
#include <vector>

#pragma comment(lib, "wsock32.lib")

#define BUF_SIZE 1024

using namespace std;

// Funkcije za slanje grešaka
void send400(SOCKET s) {
    string msg =
        "HTTP/1.0 400 Bad Request\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 15\r\n"
        "Connection: close\r\n\r\n"
        "400 Bad Request";
    send(s, msg.c_str(), msg.size(), 0);
}

void send404(SOCKET s) {
    string msg =
        "HTTP/1.0 404 Not Found\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "Connection: close\r\n\r\n"
        "404 Not Found";
    send(s, msg.c_str(), msg.size(), 0);
}

void send505(SOCKET s) {
    string msg =
        "HTTP/1.0 505 HTTP Version Not Supported\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 27\r\n"
        "Connection: close\r\n\r\n"
        "505 Version Not Supported";
    send(s, msg.c_str(), msg.size(), 0);
}

int main() {
    WSAData wsa;
    SOCKET listenSock, clientSock;
    sockaddr_in server, client;
    int clientLen = sizeof(client);
    char buffer[BUF_SIZE];

    WSAStartup(0x0202, &wsa);


    listenSock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(80);

    bind(listenSock, (sockaddr*)&server, sizeof(server));
    listen(listenSock, SOMAXCONN);

    cout << "HTTP PNG server pokrenut...\n";

    while (true) {
        clientSock = accept(listenSock, (sockaddr*)&client, &clientLen);
        cout << "Klijent povezan\n";

        int recvLen = recv(clientSock, buffer, BUF_SIZE, 0);
        if (recvLen <= 0) {
            closesocket(clientSock);
            continue;
        }

        buffer[recvLen] = '\0';
        string request(buffer);

        cout << "Primljen zahtev:\n" << request << endl;

        // Parsiranje zahteva
        string method, filename, version;
        stringstream ss(request);
        ss >> method >> filename >> version;

        // PROVERE
        if (method != "GET") {
            send400(clientSock);
            closesocket(clientSock);
            continue;
        }

        if (version != "HTTP/1.0" && version != "HTTP/1.1") {
            send505(clientSock);
            closesocket(clientSock);
            continue;
        }

        if (filename.length() < 4 || filename.substr(filename.length() - 4) != ".png") {
            send400(clientSock);
            closesocket(clientSock);
            continue;
        }

        // Uklanjanje '/'
        if (filename[0] == '/')
            filename.erase(0, 1);

        // Otvaranje PNG fajla
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            send404(clientSock);
            closesocket(clientSock);
            continue;
        }

        // Čitanje fajla
        vector<char> imageData((istreambuf_iterator<char>(file)),
            istreambuf_iterator<char>());
        file.close();

        // Slanje HTTP headera i podataka
        string header =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: image/png\r\n"
            "Content-Length: " + to_string(imageData.size()) + "\r\n"
            "Connection: close\r\n\r\n";

        send(clientSock, header.c_str(), header.size(), 0);
        send(clientSock, imageData.data(), imageData.size(), 0);

        cout << "PNG slika poslata\n";

        closesocket(clientSock);
    }

    closesocket(listenSock);
    WSACleanup();
    return 0;
}