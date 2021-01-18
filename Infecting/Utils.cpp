#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Utils.h"

#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "IPHLPAPI.lib")

void getAddrInfo(AddressInfo* info)
{
	PIP_ADAPTER_INFO pAdapterInfo = new IP_ADAPTER_INFO;
	char hostName[255];
	char* localIP;
	struct hostent* host_entry;
	in_addr paddr;

	gethostname(hostName, 255);
	host_entry = gethostbyname(hostName);
	localIP = inet_ntoa(*(struct in_addr*)*host_entry->h_addr_list);

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);

	paddr.S_un.S_addr  = ~((inet_addr(pAdapterInfo->IpAddressList.IpMask.String) | inet_addr(localIP)) ^ inet_addr(localIP));

	memcpy((char*)info->hostName, hostName, 255);
	memcpy((char*)info->ipv4, localIP, 16);
	memcpy((char*)info->netmask, pAdapterInfo->IpAddressList.IpMask.String, 16);
	memcpy((char*)info->broadcast, inet_ntoa(paddr), 16);
}
