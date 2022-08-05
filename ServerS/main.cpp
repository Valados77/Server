#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <iostream>
#include <fstream>
#include <string>
#include <experimental/filesystem>

#pragma(disable:4996)
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

void send_file(SOCKET* sock, const std::string& file_name) {
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
		delete[] bytes;
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

	sockaddr_in addr_info;
	ZeroMemory(&addr_info, sizeof(addr_info));
	int size_addr = sizeof(addr_info);
	InetPton(AF_INET, L"127.0.0.1", &addr_info.sin_addr);
	addr_info.sin_port = htons(1111);
	addr_info.sin_family = AF_INET;

	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ServSock == INVALID_SOCKET) {
		std::cout << "Error initialization socket # " << WSAGetLastError() << '\n';
		closesocket(ServSock);
		WSACleanup(); //освобождает ресурсы после WSAStartup()
		return 1;
	}
	else	std::cout << "Server socket initialization is OK" << '\n';

	if (bind(ServSock, (sockaddr*)&addr_info, sizeof(addr_info)) != 0) {
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

	SOCKET ClientConn = accept(ServSock, (sockaddr*)&addr_info, &size_addr);
	if (ClientConn == INVALID_SOCKET) {
		std::cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << '\n';
		closesocket(ServSock);
		closesocket(ClientConn);
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Connection established SUCCESSFULLY. Ready to send a picture to Client" << '\n';
	}
	std::string path;
	std::cin >> path;
	send_file(&ClientConn, path);
	system("pause");

	closesocket(ServSock);
	closesocket(ClientConn);
	WSACleanup();
	return 0;
}