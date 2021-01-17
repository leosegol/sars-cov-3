#include "Packets.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#include <time.h>  

void createDiscoverPacket(char* packet, size_t pHeader)
{
	srand(time(NULL));

	DHCP_header* p_d_dhcp = (DHCP_header*)packet[pHeader];
	memset(p_d_dhcp, 0, sizeof(p_d_dhcp));
	p_d_dhcp->op = 1;
	p_d_dhcp->htype = 1;
	p_d_dhcp->hlen = 6;
	p_d_dhcp->hops = 0;
	p_d_dhcp->xid = htonl(123456789);
	p_d_dhcp->secs = htons(0);
	p_d_dhcp->flags = htons(0x01);
	//p_d_dhcp->ciaddr = 0;
	//p_d_dhcp->yiaddr = 0;
	//p_d_dhcp->siaddr = 0;
	//p_d_dhcp->giaddr = 0;
	p_d_dhcp->chaddr[0] = rand() % 255 + 1;
	p_d_dhcp->chaddr[1] = rand() % 255 + 1;
	p_d_dhcp->chaddr[2] = rand() % 255 + 1;
	p_d_dhcp->chaddr[3] = rand() % 255 + 1;
	p_d_dhcp->chaddr[4] = rand() % 255 + 1;
	p_d_dhcp->chaddr[5] = rand() % 255 + 1;
	//p_d_dhcp->sname = { 0 };
	p_d_dhcp->magic[0] = 99;
	p_d_dhcp->magic[1] = 130;
	p_d_dhcp->magic[2] = 83;
	p_d_dhcp->magic[3] = 99;
	p_d_dhcp->opt[0] = 53;
	p_d_dhcp->opt[1] = 11;
	p_d_dhcp->opt[2] = 1;
}

void createIPv4Packet(char* packet, size_t pHeader, uint16_t total_size, PCSTR& ip)
{
	IP_header* p_ip = (IP_header*)packet[pHeader];
	p_ip->ip_header_len = 5;
	p_ip->ip_version = 4;
	p_ip->ip_tos = 0;
	p_ip->ip_total_length = total_size;
	p_ip->ip_id = htons(2);
	p_ip->ip_frag_offset = 0;
	p_ip->ip_frag_offset1 = 0;
	p_ip->ip_reserved_zero = 0;
	p_ip->ip_dont_fragment = 1;
	p_ip->ip_more_fragment = 0;
	p_ip->ip_ttl = 64;
	p_ip->ip_protocol = IPPROTO_UDP;
	inet_pton(AF_INET, "0.0.0.0", &(p_ip->ip_srcaddr));
	inet_pton(AF_INET, ip, &(p_ip->ip_destaddr));
	p_ip->ip_checksum = 0;
}





