#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

//const char SERVER_IP[] = "";					// Enter IPv4 address of Server
//const short SERVER_PORT_NUM = 0;				// Enter Listening port on Server side
const short BUFF_SIZE = 1024;

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
	servInfo.sin_port = htons(1111);

	if (connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo)) != 0) {
		std::cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << '\n';
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else std::cout << "Connection established SUCCESSFULLY. Ready to send a message to Server" << '\n';

	std::vector <char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);							// Buffers for sending and receiving data
	short packet_size = 0;												// The size of sending / receiving packet in bytes

	while (true) {

		std::cout << "Your (Client) message to Server: ";
		fgets(clientBuff.data(), clientBuff.size(), stdin);

		// Check whether client like to stop chatting 
		if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
			shutdown(ClientSock, SD_BOTH);
			closesocket(ClientSock);
			WSACleanup();
			return 0;
		}

		packet_size = send(ClientSock, clientBuff.data(), clientBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			std::cout << "Can't send message to Server. Error # " << WSAGetLastError() << '\n';
			closesocket(ClientSock);
			WSACleanup();
			return 1;
		}

		packet_size = recv(ClientSock, servBuff.data(), servBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			std::cout << "Can't receive message from Server. Error # " << WSAGetLastError() << '\n';
			closesocket(ClientSock);
			WSACleanup();
			return 1;
		}
		else
			std::cout << "Server message: " << servBuff.data() << '\n';

	}

	closesocket(ClientSock);
	WSACleanup();

	system("pause");
	return 0;
}