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
	PIP_ADAPTER_INFO pAdapterInfo{};
	PIP_ADAPTER_INFO adp{};
	PIP_ADDR_STRING address{};

	in_addr paddr;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	char hostName[255];
	char* localIP;
	struct hostent* host_entry;

	bool found = false;

	gethostname(hostName, 255);
	host_entry = gethostbyname(hostName);
	localIP = inet_ntoa(*(struct in_addr*)*host_entry->h_addr_list);

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
#if _DEBUG
		std::cout << "buffer overflow" << std::endl;
#endif
		pAdapterInfo = (PIP_ADAPTER_INFO)malloc(ulOutBufLen);
		GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
	}
	
	adp = pAdapterInfo;
	address = &pAdapterInfo->IpAddressList;

	while (adp && !found)
	{
		address = &adp->IpAddressList;
		while (address)
		{
			if (!strcmp(address->IpAddress.String, localIP))
			{
				info->htype = adp->Type;
				found = true;
				break;
			}
			address = address->Next;
		}
		adp = adp->Next;
	}

	paddr.S_un.S_addr  = ~((inet_addr(address->IpMask.String) | inet_addr(localIP)) ^ inet_addr(localIP));

	memcpy((char*)info->hostName, hostName, 255);
	memcpy((char*)info->ipv4, localIP, 16);
	memcpy((char*)info->noIP, "0.0.0.0", 16);
	memcpy((char*)info->netmask, address->IpMask.String, 16);
	memcpy((char*)info->broadcast, inet_ntoa(paddr), 16);

	free(pAdapterInfo);
}

bool checkForDHCP(DHCP_header& hDHCP)
{
	char magic[5] = { 0 };
	magic[0] = 0x63;
	magic[1] = 0x82;
	magic[2] = 0x53;
	magic[3] = 0x63;
	for (int i = 0; i < sizeof DHCP_header::magic; i++)
	{
		if (magic[i] != hDHCP.magic[i])
			return false;
	}
	return true;
}

void printHex(char* str, size_t size)
{
	uint8_t num;
	for (int i = 0; i < size; i++)
	{
		num = str[i];
		std::cout << std::hex << static_cast<unsigned>(num);
	}
	std::cout << std::endl;
}
