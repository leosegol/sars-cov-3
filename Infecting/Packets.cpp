#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Headers.h"
#include "Utils.h"
#include <iostream>
#include <WS2tcpip.h>
#pragma pack(1)
#pragma comment(lib, "Ws2_32.lib")



void CreateDHCPDiscoverPacket(char* raw_packet, AddressInfo& info)
{
	createIPv4Header(
			raw_packet,
			0,
			sizeof(IP_header) + sizeof(DHCP_header) + sizeof(UDP_header),
			info.noIP,
			info.broadcast
	);

	createUDPHeader(
			raw_packet,
			sizeof(IP_header),
			68, 67,
			sizeof(UDP_header) + sizeof(DHCP_header)
	);

	createDHCPdiscoverHeader(
			raw_packet,
			sizeof(IP_header) + sizeof(UDP_header),
			info.htype
	);
}

void CreateDHCPOfferPacket(char* raw_packet, void* pDHCP , AddressInfo& info)
{
	DHCP_header hDHCP = *(DHCP_header*)pDHCP;
	sockaddr_in toStr;
	toStr.sin_addr.s_addr = createRandomIP(info);

	createEthernetHeader(raw_packet,
		0,
		(uint8_t*)hDHCP.chaddr,
		info.byteMac
	);

	createIPv4Header(
		raw_packet,
		sizeof(Ethernet_header),
		sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(DHCP_header) + sizeof(UDP_header) -14,
		info.gateWay,
		(uint8_t*)inet_ntoa(toStr.sin_addr)
	);

	createUDPHeader(
		raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header),
		67, 68,
		sizeof(UDP_header) + sizeof(DHCP_header)
	);
	std::cout << "from inside " << htons(((UDP_header*)(raw_packet + sizeof(Ethernet_header) + sizeof(IP_header)))->dst_port) << std::endl;

	createDHCPOfferHeader(
		raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header),
		hDHCP,
		info,
		toStr.sin_addr.s_addr
	);
}

void getDHCPPacketInfo(char* packet, DHCP_header& pDHCP, UDP_header& pUDP, IP_header& pIP)
{
	getIPheader(
		packet,
		0,
		pIP
	);

	getUDPheader(
		packet,
		sizeof(IP_header),
		pUDP
	);

	getDHCPheader(
		packet,
		sizeof(IP_header) + sizeof(UDP_header),
		pDHCP
	);
}
