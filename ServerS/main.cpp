#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <experimental/filesystem>

#pragma comment(lib, "Ws2_32.lib")

//const char IP_SERV[] = "";			// Enter local Server IP address
//const int PORT_NUM = 0;				// Enter Open working server port
const short BUFF_SIZE = 1024;

void SendFile(SOCKET* sock, const std::string& file_name) {
	std::fstream file;
	file.open(file_name, std::ios_base::in | std::ios_base::binary);
	if (file.is_open()) {
		int file_size = std::experimental::filesystem::file_size(file_name) + 1;
		char* bytes = new char[file_size];
		file.read(bytes, file_size);

		std::cout << "size: " << file_size << std::endl;
		std::cout << "name: " << file_name << std::endl;
		std::cout << "data: " << bytes << std::endl;

		send(*sock, std::to_string(file_size).c_str(), 16, 0);
		send(*sock, file_name.c_str(), 32, 0);
		send(*sock, bytes, file_size, 0);
	}
	else std::cout << "Error file open\n";
		file.close();
}

int main() {
	WSADATA wsData; // структура содержащая детали реализации Windows Sockets
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) { //a-максимальная версия Windows Sockets; b-указатель на структуру WSADATA
		//wsData получает информацию о версии WinSock 
		std::cout << "Error WinSock version initializaion #" << WSAGetLastError();
		return 1;
	}
	else	std::cout << "WinSock initialization is OK" << '\n';

	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ServSock == INVALID_SOCKET) {
		std::cout << "Error initialization socket # " << WSAGetLastError() << '\n';
		closesocket(ServSock);
		WSACleanup(); //освобождает ресурсы после WSAStartup()
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
	//servInfo.sin_addr = ip_to_num;
	servInfo.sin_addr.S_un.S_addr = INADDR_ANY;
	servInfo.sin_port = htons(1111);

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
	else {
		std::cout << "Connection to a client established successfully" << '\n';
		std::string path;
		std::cin >> path;
		SendFile(&ClientConn, path);
	}

	//std::vector <char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);							// Creation of buffers for sending and receiving data
	//short packet_size = 0;												// The size of sending / receiving packet in bytes

	//while (true) {
	//	packet_size = recv(ClientConn, servBuff.data(), servBuff.size(), 0);					// Receiving packet from client. Program is waiting (system pause) until receive
	//	std::cout << "Client's message: " << servBuff.data() << '\n';

	//	std::cout << "Your (host) message: ";
	//	fgets(clientBuff.data(), clientBuff.size(), stdin);

	//	// Check whether server would like to stop chatting 
	//	if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
	//		shutdown(ClientConn, SD_BOTH);
	//		closesocket(ServSock);
	//		closesocket(ClientConn);
	//		WSACleanup();
	//		return 0;
	//	}
	//	else if (clientBuff[0] == 'p' && clientBuff[1] == 'n' && clientBuff[2] == 'j') {

	//		shutdown(ClientConn, SD_BOTH);
	//		closesocket(ServSock);
	//		closesocket(ClientConn);
	//		WSACleanup();
	//		return 0;
	//	}

	//	packet_size = send(ClientConn, clientBuff.data(), clientBuff.size(), 0);

	//	if (packet_size == SOCKET_ERROR) {
	//		std::cout << "Can't send message to Client. Error # " << WSAGetLastError() << '\n';
	//		closesocket(ServSock);
	//		closesocket(ClientConn);
	//		WSACleanup();
	//		return 1;
	//	}

	//}

	closesocket(ServSock);
	closesocket(ClientConn);
	WSACleanup();


	system("pause");
	return 0;
}