#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Headers.h"
#include "Utils.h"
#include <iostream>
#include <WS2tcpip.h>
#pragma pack(1)
#pragma comment(lib, "Ws2_32.lib")

#include <random>

void createDHCPDiscoverPacket(char* raw_packet, AddressInfo& info)
{


	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> rand(1, 65536);

	createIPv4Header(
			raw_packet,
			0,
			sizeof(IP_header) + sizeof(DHCP_header) + sizeof(UDP_header),
			info.noIP,
			info.broadcast,
			htons((uint16_t)rand(rng))
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

void createDHCPOfferPacket(char* raw_packet, void* pDHCP, void* pIP , AddressInfo& info)
{
	DHCP_header hDHCP = *(DHCP_header*)pDHCP;
	IP_header hIP = *(IP_header*)pIP;
	sockaddr_in toStr;
	toStr.sin_addr.s_addr = createRandomIP(info);
	printIP(toStr.sin_addr.s_addr);
	createEthernetHeader(
		raw_packet,
		0,
		(uint8_t*)hDHCP.chaddr,
		info.byteMac
	);

	createIPv4Header(
		raw_packet,
		sizeof(Ethernet_header),
		sizeof(IP_header) + sizeof(DHCP_header) + sizeof(UDP_header),
		info.gateWay,
		(uint8_t*)inet_ntoa(toStr.sin_addr),
		hIP.ip_id
	);

	createUDPHeader(
		raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header),
		67, 68,
		sizeof(UDP_header) + sizeof(DHCP_header)
	);

	createDHCPOfferHeader(
		raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header),
		hDHCP,
		info,
		toStr.sin_addr.s_addr
	);
}

void createDHCPackPacket(char* raw_packet, void* pDHCP, void* pIP, AddressInfo& info)
{
	DHCP_header hDHCP = *(DHCP_header*)pDHCP;
	IP_header hIP = *(IP_header*)pIP;
	sockaddr_in toStr;
	toStr.sin_addr.s_addr = getRequestedIP(*(DHCP_header*)pDHCP);

	createEthernetHeader(
		raw_packet,
		0,
		(uint8_t*)hDHCP.chaddr,
		info.byteMac
	);

	createIPv4Header(
		raw_packet,
		sizeof(Ethernet_header),
		sizeof(IP_header) + sizeof(DHCP_header) + sizeof(UDP_header),
		info.gateWay,
		(uint8_t*)inet_ntoa(toStr.sin_addr),
		hIP.ip_id
	);

	createUDPHeader(
		raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header),
		67, 68,
		sizeof(UDP_header) + sizeof(DHCP_header)
	);

	createDHCPackHeader(
		raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header),
		hDHCP,
		info
	);
}

int createDNSResponsePacket(char* raw_packet, void* pDNS, void* pIP, void* pUDP, void* pQuery, void* pEther,AddressInfo& info)
{
	DNS_header dns = *(DNS_header*)pDNS;
	IP_header ip = *(IP_header*)pIP;
	UDP_header udp = *(UDP_header*)pIP;
	DNS_query* query = (DNS_query*)pQuery;
	Ethernet_header ether = *(Ethernet_header*)pEther;

	sockaddr_in toStr1;
	sockaddr_in toStr2;
	DNS_record record;

	toStr1.sin_addr.s_addr = ip.ip_destaddr;
	toStr2.sin_addr.s_addr = ip.ip_srcaddr;
	DNS_record* records = (DNS_record*)malloc(dns.questions*sizeof(DNS_record));

	uint16_t ptr = 0b1100000000000000;	// saying its a pointer

	for (int i = 0; i < dns.questions; i++)
	{
		records[i].name = (uint8_t*)(ptr | (sizeof(DNS_header) + sizeof(DNS_query) * i));	// pointing to the name
		records[i].resource->type = htons(1);
		records[i].resource->_class = htons(1);
		records[i].resource->ttl = htonl(100);
		records[i].resource->data_len = htons(4);
		records[i].rdata = (uint8_t*)inet_addr((const char*)info.ipv4);
	}

	createEthernetHeader(
		raw_packet,
		0,
		ether.src_addr,
		ether.dest_addr
	);

	createIPv4Header(
		raw_packet,
		sizeof(Ethernet_header),
		sizeof(IP_header) + sizeof(UDP_header) + sizeof(DNS_header) + sizeof(DNS_query)*dns.questions + sizeof(DNS_record)*dns.questions,
		(uint8_t*)inet_ntoa(toStr1.sin_addr),
		(uint8_t*)inet_ntoa(toStr2.sin_addr),
		ip.ip_id
	);

	createUDPHeader(
		raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header),
		udp.dst_port,
		udp.src_port,
		sizeof(UDP_header) + sizeof(DNS_header) + sizeof(DNS_query) * dns.questions + sizeof(DNS_record) * dns.questions
	);

	createDNSResponseHeader(
		raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header),
		dns,
		inet_addr((const char*)info.ipv4)
	);

	memcpy(
		&raw_packet[sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header) + sizeof(DNS_header)],
		(const char*)query,
		sizeof(DNS_query) * dns.questions
	);

	memcpy(
		&raw_packet[sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header) + sizeof(DNS_header) + sizeof(DNS_query) * dns.questions],
		(const char*)records,
		sizeof(DNS_record) * dns.questions
	);

	return sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header) + 
		sizeof(DNS_header) + sizeof(DNS_query) * dns.questions + 
		sizeof(DNS_record) * dns.questions;
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
