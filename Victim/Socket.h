#pragma once
#include<iostream>
#include<WinSock2.h>
#include<Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

enum opt
{
	TELNET = 0,
	OTHER
};

class Socket
{
public:
	const char* ip;
	short port;
	SOCKET sock;

private:
	sockaddr_in addr_info;

	WSADATA wsaData;

public:
	Socket();

	int Connect(const char* ip, short port);

	int TelNet();

	int Read();

	int Interact();

	void Send(std::string);
};