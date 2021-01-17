#include "Packets.h"
#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

struct DHCP_header createDiscoverPacket(char* mac)
{
	DHCP_header d_dhcp;
	memset(&d_dhcp, 0, sizeof(d_dhcp)); // 
	d_dhcp.op = 1;
	d_dhcp.htype = 1;
	d_dhcp.hlen = 6;
	d_dhcp.hops = 0;
	d_dhcp.xid = htonl(123456789);
	d_dhcp.secs = htons(0);
	d_dhcp.flags = htons(0x01);
	//d_dhcp.ciaddr = 0;
	//d_dhcp.yiaddr = 0;
	//d_dhcp.siaddr = 0;
	//d_dhcp.giaddr = 0;
	d_dhcp.chaddr[0] = mac[0];
	d_dhcp.chaddr[1] = mac[1];
	d_dhcp.chaddr[2] = mac[2];
	d_dhcp.chaddr[3] = mac[3];
	d_dhcp.chaddr[4] = mac[4];
	d_dhcp.chaddr[5] = mac[5];
	//d_dhcp.sname = { 0 };
	d_dhcp.magic[0] = 99;
	d_dhcp.magic[1] = 130;
	d_dhcp.magic[2] = 83;
	d_dhcp.magic[3] = 99;
	d_dhcp.opt[0] = 53;
	d_dhcp.opt[1] = 11;
	d_dhcp.opt[2] = 1;
	return d_dhcp;
}

