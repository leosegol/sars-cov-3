#include "Utils.h"
#include <Windows.h>

#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
uint32_t getBroadcastIP()
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
				found = true;
				break;
			}
			address = address->Next;
		}
		adp = adp->Next;
	}

	paddr.S_un.S_addr = ~((inet_addr(address->IpMask.String) | inet_addr(localIP)) ^ inet_addr(localIP));
	free(pAdapterInfo);
	return paddr.s_addr;
}

void printVictims(uint32_t* address)
{
	std::string nextline;
	int i = 0;
	in_addr addr;

	nextline = "List of victims:\n";
	while (address[i] != NULL)
	{
		addr.s_addr = address[i];
		std::cout << "[" << ++i << "] " << inet_ntoa(addr) << '\n';
	}
	std::cout << std::endl;
}

uint32_t chooseVictim(uint32_t* address)
{
	int victimIndex;
	do
	{
		std::cout << "Enter victim by number> ";
		std::cin >> victimIndex;
		std::cout << std::endl;
	} while (address[victimIndex] == NULL);
	return address[victimIndex];
}
