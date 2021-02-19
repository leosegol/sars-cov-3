#pragma once
#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>

uint32_t broadcastIP();

uint32_t getBroadcastIP();

void printVictims(uint32_t* address);

uint32_t chooseVictim(uint32_t* address);