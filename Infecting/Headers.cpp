#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Packets.h"
#include "Headers.h"
#include "Utils.h"

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Ws2_32.lib")

#include <random>


void createDHCPdiscoverHeader(char* packet, size_t pHeader, uint8_t htype)
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist255(1, 255);

	DHCP_header* p_d_dhcp = (DHCP_header*)&packet[pHeader];
	memset(p_d_dhcp, 0, sizeof(DHCP_header));

	p_d_dhcp->op = 1;
	if((int)htype == IF_TYPE_IEEE80211)
		p_d_dhcp->htype = 6;
	else
		p_d_dhcp->htype = 1;
	p_d_dhcp->hlen = 6;
	p_d_dhcp->hops = 0;
	p_d_dhcp->xid = htonl(123456789);
	p_d_dhcp->secs = htons(0);
	p_d_dhcp->flags = htons(0x00);
	//p_d_dhcp->ciaddr = 0;
	//p_d_dhcp->yiaddr = 0;
	//p_d_dhcp->siaddr = 0;
	//p_d_dhcp->giaddr = 0;
	p_d_dhcp->chaddr[0] = (unsigned char)dist255(rng);
	p_d_dhcp->chaddr[1] = (unsigned char)dist255(rng);
	p_d_dhcp->chaddr[2] = (unsigned char)dist255(rng);
	p_d_dhcp->chaddr[3] = (unsigned char)dist255(rng);
	p_d_dhcp->chaddr[4] = (unsigned char)dist255(rng);
	p_d_dhcp->chaddr[5] = (unsigned char)dist255(rng);
	//p_d_dhcp->sname = 0;
	//p_d_dhcp->file = { 0 };
	p_d_dhcp->magic[0] = 99;
	p_d_dhcp->magic[1] = 130;
	p_d_dhcp->magic[2] = 83;
	p_d_dhcp->magic[3] = 99;
	p_d_dhcp->opt[0] = 53;						// type of packet
	p_d_dhcp->opt[1] = 1;
	p_d_dhcp->opt[2] = 1;
	p_d_dhcp->opt[3] = 61;						// htype
	p_d_dhcp->opt[4] = 7;
	if ((int)htype == IF_TYPE_IEEE80211)
		p_d_dhcp->opt[5] = 6;
	else
		p_d_dhcp->opt[5] = 1;
	p_d_dhcp->opt[6] = p_d_dhcp->chaddr[0];
	p_d_dhcp->opt[7] = p_d_dhcp->chaddr[1];
	p_d_dhcp->opt[8] = p_d_dhcp->chaddr[2];
	p_d_dhcp->opt[9] = p_d_dhcp->chaddr[3];
	p_d_dhcp->opt[10] = p_d_dhcp->chaddr[4];
	p_d_dhcp->opt[11] = p_d_dhcp->chaddr[5];
	p_d_dhcp->opt[12] = 57;						//max packet size
	p_d_dhcp->opt[13] = 2;
	p_d_dhcp->opt[14] = htons(1500);
	//p_d_dhcp->opt[15]; taken
	p_d_dhcp->opt[16] = 255;					// end of opt
}

void createIPv4Header(char* packet, size_t pHeader, uint16_t total_size, uint8_t* src, uint8_t* dst, uint16_t id)
{
	IP_header* p_ip = (IP_header*)&packet[pHeader];
	p_ip->ip_header_len = 5;
	p_ip->ip_version = 4;
	p_ip->ip_tos = 0;
	p_ip->ip_total_length = htons(total_size);
	p_ip->ip_id = id;
	p_ip->ip_frag_offset = 0;
	p_ip->ip_frag_offset1 = 0;
	p_ip->ip_reserved_zero = 0;
	p_ip->ip_dont_fragment = 0;
	p_ip->ip_more_fragment = 0;
	p_ip->ip_ttl = 64;
	p_ip->ip_protocol = IPPROTO_UDP;
	inet_pton(AF_INET, (char*)src, &(p_ip->ip_srcaddr));
	inet_pton(AF_INET, (char*)dst, &(p_ip->ip_destaddr));
	p_ip->ip_checksum = 0;
	p_ip->ip_checksum = in_checksum((uint16_t*)p_ip, sizeof(IP_header));
}

void createUDPHeader(char* packet, size_t pHeader, uint16_t src_port, uint16_t dst_port, uint16_t p_size)
{
	UDP_header* p_udp = (UDP_header*)&packet[pHeader];
	p_udp->src_port = htons(src_port);
	p_udp->dst_port = htons(dst_port);
	p_udp->len = htons(p_size);
	p_udp->chksum = 0;
}

void createEthernetHeader(char* packet, size_t pHeader, uint8_t* dst_mac, uint8_t* src_mac)
{
	Ethernet_header* p_ether = (Ethernet_header*)&packet[pHeader];
	p_ether->dest_addr[0] = dst_mac[0];
	p_ether->dest_addr[1] = dst_mac[1];
	p_ether->dest_addr[2] = dst_mac[2];
	p_ether->dest_addr[3] = dst_mac[3];
	p_ether->dest_addr[4] = dst_mac[4];
	p_ether->dest_addr[5] = dst_mac[5];

	p_ether->src_addr[0] = src_mac[0];
	p_ether->src_addr[1] = src_mac[1];
	p_ether->src_addr[2] = src_mac[2];
	p_ether->src_addr[3] = src_mac[3];
	p_ether->src_addr[4] = src_mac[4];
	p_ether->src_addr[5] = src_mac[5];

	p_ether->frame_type = htons(0x0800);
}

void getDHCPheader(char* packet, size_t pHeader, DHCP_header& rDHCP)
{
	DHCP_header* hDHCP = (DHCP_header*)&packet[pHeader];
	if(hDHCP)
		rDHCP = *hDHCP;
}

void getIPheader(char* packet, size_t pHeader, IP_header& rIP)
{
	IP_header* hIP = (IP_header*)&packet[pHeader];
	if(hIP)
		rIP = *hIP;
}

void getUDPheader(char* packet, size_t pHeader, UDP_header& rUDP)
{
	UDP_header* hUDP = (UDP_header*)&packet[pHeader];
	if(hUDP)
		rUDP = *hUDP;
}

void getDNSheader(char* packet, size_t pHeader, DNS_header& rDNS)
{
	DNS_header* hDNS = (DNS_header*)&packet[pHeader];
	if(hDNS)
		rDNS = *hDNS;
}

void getEthernetheader(char* packet, size_t pHeader, Ethernet_header& rEther)
{
	Ethernet_header* hEther = (Ethernet_header*)&packet[pHeader];
	if (hEther)
		rEther = *hEther;
}

void createDHCPOfferHeader(char* packet, size_t pHeader, DHCP_header& pDiscover, AddressInfo& info, uint32_t randomIP)
{
	DHCP_header* p_d_dhcp = (DHCP_header*)&packet[pHeader];

	memset(p_d_dhcp, 0, sizeof(DHCP_header));

	p_d_dhcp->op = 2;
	p_d_dhcp->htype = 1;
	p_d_dhcp->hlen = 6;
	p_d_dhcp->hops = 0;
	p_d_dhcp->xid = pDiscover.xid;
	p_d_dhcp->secs = htons(0);
	p_d_dhcp->flags = htons(0x00);
	//p_d_dhcp->ciaddr = 0;
	p_d_dhcp->yiaddr = randomIP;
	//p_d_dhcp->siaddr = 0;
	//p_d_dhcp->giaddr = 0;
	p_d_dhcp->chaddr[0] = pDiscover.chaddr[0];
	p_d_dhcp->chaddr[1] = pDiscover.chaddr[1];
	p_d_dhcp->chaddr[2] = pDiscover.chaddr[2];
	p_d_dhcp->chaddr[3] = pDiscover.chaddr[3];
	p_d_dhcp->chaddr[4] = pDiscover.chaddr[4];
	p_d_dhcp->chaddr[5] = pDiscover.chaddr[5];
	//p_d_dhcp->sname = 0;
	//p_d_dhcp->file = { 0 };
	p_d_dhcp->magic[0] = 99;
	p_d_dhcp->magic[1] = 130;
	p_d_dhcp->magic[2] = 83;
	p_d_dhcp->magic[3] = 99;

	p_d_dhcp->opt[0] = 53;		// Message Type
	p_d_dhcp->opt[1] = 1;
	p_d_dhcp->opt[2] = 2;

	p_d_dhcp->opt[3] = 54;		// DHCP server's ip
	p_d_dhcp->opt[4] = 4;
	*((uint32_t*)&p_d_dhcp->opt[5]) = inet_addr((const char*)info.gateWay);
	//p_d_dhcp->opt[6];
	//p_d_dhcp->opt[7];				// taken
	//p_d_dhcp->opt[8];

	p_d_dhcp->opt[9] = 51;		// lease time
	p_d_dhcp->opt[10] = 4;
	*((uint32_t*)&p_d_dhcp->opt[11]) = htonl(3600);
	//p_d_dhcp->opt[12];
	//p_d_dhcp->opt[13];				// taken
	//p_d_dhcp->opt[14];

	p_d_dhcp->opt[15] = 1;		// netMask
	p_d_dhcp->opt[16] = 4;
	*((uint32_t*)&p_d_dhcp->opt[17]) = inet_addr((const char*)info.netmask);
	//p_d_dhcp->opt[18];
	//p_d_dhcp->opt[19];					// taken
	//p_d_dhcp->opt[20];

	p_d_dhcp->opt[21] = 3;		// router ip
	p_d_dhcp->opt[22] = 4;
	*((uint32_t*)&p_d_dhcp->opt[23]) = inet_addr((const char*)info.gateWay); // tempotal
	//p_d_dhcp->opt[24];
	//p_d_dhcp->opt[25];				// taken
	//p_d_dhcp->opt[26];

	p_d_dhcp->opt[27] = 6;		// DNS
	p_d_dhcp->opt[28] = 4;
	*((uint32_t*)&p_d_dhcp->opt[29]) = inet_addr((const char*)info.ipv4);
	//p_d_dhcp->opt[40];
	//p_d_dhcp->opt[32];				// taken
	//p_d_dhcp->opt[32];

	p_d_dhcp->opt[33] = 15;		// Domain name
	p_d_dhcp->opt[34] = 4;
	*((uint32_t*)&p_d_dhcp->opt[35]) = htonl(*(uint32_t*)&info.domainName);
	//p_d_dhcp->opt[36];
	//p_d_dhcp->opt[37];				// taken
	//p_d_dhcp->opt[38];

	p_d_dhcp->opt[39] = 255;
}

void createDHCPackHeader(char* packet, size_t pHeader, DHCP_header& pRequest, AddressInfo& info)
{
	DHCP_header* p_d_dhcp = (DHCP_header*)&packet[pHeader];

	memset(p_d_dhcp, 0, sizeof(DHCP_header));

	p_d_dhcp->op = 2;
	p_d_dhcp->htype = 1;
	p_d_dhcp->hlen = 6;
	p_d_dhcp->hops = 0;
	p_d_dhcp->xid = pRequest.xid;
	p_d_dhcp->secs = htons(0);
	p_d_dhcp->flags = htons(0x00);
	//p_d_dhcp->ciaddr = 0;
	p_d_dhcp->yiaddr = getRequestedIP(pRequest);
	//p_d_dhcp->siaddr = 0;
	//p_d_dhcp->giaddr = 0;
	p_d_dhcp->chaddr[0] = pRequest.chaddr[0];
	p_d_dhcp->chaddr[1] = pRequest.chaddr[1];
	p_d_dhcp->chaddr[2] = pRequest.chaddr[2];
	p_d_dhcp->chaddr[3] = pRequest.chaddr[3];
	p_d_dhcp->chaddr[4] = pRequest.chaddr[4];
	p_d_dhcp->chaddr[5] = pRequest.chaddr[5];
	//p_d_dhcp->sname = 0;
	//p_d_dhcp->file = { 0 };
	p_d_dhcp->magic[0] = 99;
	p_d_dhcp->magic[1] = 130;		//DHCP
	p_d_dhcp->magic[2] = 83;
	p_d_dhcp->magic[3] = 99;

	p_d_dhcp->opt[0] = 53;		// Message Type
	p_d_dhcp->opt[1] = 1;
	p_d_dhcp->opt[2] = 5;

	p_d_dhcp->opt[3] = 54;		// DHCP server's ip
	p_d_dhcp->opt[4] = 4;
	*((uint32_t*)&p_d_dhcp->opt[5]) = inet_addr((const char*)info.gateWay);
	//p_d_dhcp->opt[6];
	//p_d_dhcp->opt[7];				// taken
	//p_d_dhcp->opt[8];

	p_d_dhcp->opt[9] = 51;		// lease time
	p_d_dhcp->opt[10] = 4;
	*((uint32_t*)&p_d_dhcp->opt[11]) = htonl(3600);
	//p_d_dhcp->opt[12];
	//p_d_dhcp->opt[13];				// taken
	//p_d_dhcp->opt[14];

	p_d_dhcp->opt[15] = 1;		// netMask
	p_d_dhcp->opt[16] = 4;
	*((uint32_t*)&p_d_dhcp->opt[17]) = inet_addr((const char*)info.netmask);
	//p_d_dhcp->opt[18];
	//p_d_dhcp->opt[19];					// taken
	//p_d_dhcp->opt[20];

	p_d_dhcp->opt[21] = 3;		// router ip
	p_d_dhcp->opt[22] = 4;
	*((uint32_t*)&p_d_dhcp->opt[23]) = inet_addr((const char*)info.gateWay); // tempotal
	//p_d_dhcp->opt[24];
	//p_d_dhcp->opt[25];				// taken
	//p_d_dhcp->opt[26];

	p_d_dhcp->opt[27] = 6;		// DNS
	p_d_dhcp->opt[28] = 4;
	*((uint32_t*)&p_d_dhcp->opt[29]) = inet_addr((const char*)info.ipv4);
	//p_d_dhcp->opt[40];
	//p_d_dhcp->opt[32];				// taken
	//p_d_dhcp->opt[32];

	p_d_dhcp->opt[33] = 15;		// Domain name
	p_d_dhcp->opt[34] = 4;
	*((uint32_t*)&p_d_dhcp->opt[35]) = htonl(*(uint32_t*)&info.domainName);
	//p_d_dhcp->opt[36];
	//p_d_dhcp->opt[37];				// taken
	//p_d_dhcp->opt[38];

	p_d_dhcp->opt[39] = 255;
}

void createDNSResponseHeader(char* packet, size_t pHeader, DNS_header& rDNS, uint32_t dstIP)
{
	DNS_header* p_dns = (DNS_header*)&packet[pHeader];

	p_dns->id = rDNS.id;
	p_dns->flags = htons(0x8000);
	p_dns->questions = rDNS.questions;
	p_dns->answers = htons(1);
	p_dns->authority = 0;
	p_dns->additional = 0;
}