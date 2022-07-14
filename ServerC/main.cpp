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

	SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSock == INVALID_SOCKET) {
		std::cout << "Error initialization socket # " << WSAGetLastError() << '\n';
		closesocket(ClientSock);
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

	if (connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo)) != 0) {
		std::cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << '\n';
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else std::cout << "Connection established SUCCESSFULLY. Ready to send a message to Server" << '\n';

	system("pause");
	return 0;
}