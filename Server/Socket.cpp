#include"socket.h"
#include<winsock2.h>
#include<Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

Socket::Socket(const char* ip, short port)
{
	this->ip = ip;
	this->port = port;

	this->addr_info.sin_family = AF_INET; // using IPv4
	this->addr_info.sin_port = htons(port); // choosing port
	//addr_info.sin_addr.s_addr = inet_addr(ip);
	inet_pton(AF_INET, ip, &addr_info.sin_addr.s_addr);

	char buffer[INET_ADDRSTRLEN];
	//inet_ntop(AF_INET, &addr_info.sin_addr.s_addr, buffer, sizeof(buffer));

	WSAStartup(MAKEWORD(2, 2), &wsaData); // tracks the errors
	this->sock = socket(AF_INET, SOCK_STREAM, 0);
	if (this->sock == INVALID_SOCKET) // create socket
		std::cout << "socket could not have been built " << WSAGetLastError() << std::endl;

	if (bind(this->sock, (const sockaddr*)&addr_info, sizeof(addr_info)) == SOCKET_ERROR)
		std::cout << "could not bind ip" << buffer << " and port " << port << " to the socket " << WSAGetLastError() << std::endl;

	int optval = 0; // the func must get a pointer so i create optval
	//if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, 0, sizeof(sizeof(int))) == SOCKET_ERROR)
		//std::cout << "could not make the port " << port << " reuseable " << WSAGetLastError() << std::endl;

	if (listen(this->sock, 1) == SOCKET_ERROR)
		std::cout << "could not listen " << WSAGetLastError() << std::endl;

}

SOCKET Socket::acccept(struct sockaddr_in& addr)
{
	//sockaddr_in addr;
	int len = sizeof(addr);
	SOCKET ret = accept(sock, (sockaddr*)&addr, &len);
	if (ret == INVALID_SOCKET)
		std::cout << "connection error " << WSAGetLastError();
	//else
		//std::cout << inet_ntoa(addr.sin_addr) << " joined" << std::endl;
	return ret;
}

