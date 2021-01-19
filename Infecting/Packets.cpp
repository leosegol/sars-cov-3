#include "Packets.h"
#include "Headers.h"
#include <iostream>
#include <winsock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#pragma pack(1)
#pragma comment(lib, "Ws2_32.lib")



void CreateDHCPDiscoverPacket(char* raw_packet, AddressInfo& info)
{
	createIPv4Header(
			raw_packet,
			0,
			sizeof(IP_header) + sizeof(DHCP_header) + sizeof(UDP_header),
			info.ipv4,
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
			sizeof(IP_header) + sizeof(UDP_header)
	);
}
