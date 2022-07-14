#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

int main() {
	WSADATA wsData;
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
		std::cout << "Error WinSock version initializaion #" << WSAGetLastError();
		return 1;
	}
	else	std::cout << "WinSock initialization is OK" << '\n';

	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ServSock == INVALID_SOCKET) {
		std::cout << "Error initialization socket # " << WSAGetLastError() << '\n';
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else	std::cout << "Server socket initialization is OK" << '\n';

	in_addr ip_to_num;
	if (inet_pton(AF_INET, "127.0.0.1", &ip_to_num) <= 0) {
		std::cout << "Error in IP translation to special numeric format" << '\n';
		return 1;
	}

	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));
	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(1234);

	if (bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo)) != 0) {
		std::cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << '\n';
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else	std::cout << "Binding socket to Server info is OK" << '\n';

	if (listen(ServSock, SOMAXCONN) != 0) {
		std::cout << "Can't start to listen to. Error # " << WSAGetLastError() << '\n';
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else	std::cout << "Listening..." << '\n';

	sockaddr_in clientInfo;
	ZeroMemory(&clientInfo, sizeof(clientInfo));
	int clientInfo_size = sizeof(clientInfo);
	SOCKET ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);
	if (ClientConn == INVALID_SOCKET) {
		std::cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << '\n';
		closesocket(ServSock);
		closesocket(ClientConn);
		WSACleanup();
		return 1;
	}
	else	std::cout << "Connection to a client established successfully" << '\n';

	system("pause");
	return 0;
}