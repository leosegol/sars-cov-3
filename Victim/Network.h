#pragma once
#include<iostream>
#include<WinSock2.h>
#include<Ws2tcpip.h>
#include<windows.h>

uint32_t findMastersIP();

int sendStdOut(SOCKET s, std::string& message);
