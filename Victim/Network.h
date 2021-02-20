#pragma once
#include<iostream>
#include<WinSock2.h>
#include<Ws2tcpip.h>
#include<windows.h>

enum protocol {
	REQ = 8569,
	RPY = 4586
};


#define UDP_PORT 32568
#define TCP_PORT 32569

uint32_t findMastersIP();

int sendStdOut(SOCKET s, std::string& message);
