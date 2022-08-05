#include <iostream>
#include <fstream>
#include <string>

#pragma warning(disable: 4996)
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

void recv_file(SOCKET* sock) {
	char file_size_str[16];
	char file_name[32];

	recv(*sock, file_size_str, 16, 0);
	int file_size = atoi(file_size_str);
	char* bytes = new char[file_size];

	recv(*sock, file_name, 32, 0);

	std::fstream file;
	file.open(file_name, std::ios_base::out | std::ios_base::binary);

	std::cout << "size: " << file_size << '\n';
	std::cout << "name: " << file_name << '\n';

	if (file.is_open()) {
		recv(*sock, bytes, file_size, 0);
		std::cout << "data: " << bytes << '\n';

		file.write(bytes, file_size);
		std::cout << "ok save\n";
	}
	else std::cout << "Error file open\n";

	delete[] bytes;
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

	SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ClientSock == INVALID_SOCKET) {
		std::cout << "Error initialization socket # " << WSAGetLastError() << '\n';
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else	std::cout << "Server socket initialization is OK" << '\n';

	sockaddr_in addr_info;
	ZeroMemory(&addr_info, sizeof(addr_info));

	int size_addr = sizeof(addr_info);
	InetPton(AF_INET, L"127.0.0.1", &addr_info.sin_addr);
	addr_info.sin_port = htons(1111);
	addr_info.sin_family = AF_INET;


	if (connect(ClientSock, (sockaddr*)&addr_info, sizeof(addr_info)) != 0) {
		std::cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << '\n';
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else {
		std::cout << "Connection established SUCCESSFULLY. Ready to receive a picture from Server" << '\n';
	}
	recv_file(&ClientSock);
	system("pause");

	closesocket(ClientSock);
	WSACleanup();
	return 0;
}