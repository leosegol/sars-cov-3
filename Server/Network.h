#pragma once
#include<iostream>
#include<WinSock2.h>
#include<Ws2tcpip.h>

void searchVictims(uint32_t* address);

std::string telNet(SOCKET s, std::string& command);

