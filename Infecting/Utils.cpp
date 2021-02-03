#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define HAVE_REMOTE
#define WPCAP

#include "Headers.h"
#include "Packets.h"
#include "Utils.h"

#include <pcap.h>
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "packet.lib")

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

	FIXED_INFO* pFixedInfo{};

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

	if (GetNetworkParams(pFixedInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
		pFixedInfo = (FIXED_INFO*)malloc(ulOutBufLen);


	
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
				memcpy((char*)info->gateWay, adp->GatewayList.IpAddress.String, 16);
				memcpy((char*)info->AdaptersName, adp->AdapterName, 260);
				for (int i = 0; i < sizeof info->byteMac; i++)
					info->byteMac[i] = (uint8_t)adp->Address[i];
				info->Index = adp->Index;
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
	memcpy((char*)info->domainName, pFixedInfo->DomainName, 4);
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
	return hDHCP.op == 1;
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

void printIP(uint32_t byteIP)
{
	sockaddr_in toStr;
	toStr.sin_addr.s_addr = byteIP;
	std::cout << inet_ntoa(toStr.sin_addr) << std::endl;
}

uint32_t createRandomIP(AddressInfo& info)
{
	uint32_t subnet, ip, temp, randomIP;
	subnet = inet_addr((const char*)info.netmask);
	ip = inet_addr((const char*)info.ipv4);
	srand(time(NULL));

	do
	{
		temp = ~subnet | ((subnet | ip) ^ subnet);
		randomIP = htonl(ntohl(temp) ^ (rand() % ntohl(~subnet))) ^ ip;
	} while (randomIP == ~((subnet | ip) ^ ip));
	return randomIP;
}

const char* getDeviceName(AddressInfo& info)
{
	pcap_if_t* alldevs;
	pcap_if_t* d;
	std::string name = "";
	int i = 0;
	char errbuf[PCAP_ERRBUF_SIZE];

	if (pcap_findalldevs_ex((char*)PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		std::cout << "error finding devs" << std::endl;
		return NULL;
	}

	for (d = alldevs; d != NULL; d = d->next)
	{
		name = std::string(d->name);
			if(name.substr(name.find_first_of("{"))._Equal(std::string((char*)info.AdaptersName)))
				return d->name;
	}

	return NULL;
}

uint16_t in_checksum(unsigned short* ptr, int nbytes)
{
	register long sum;
	unsigned short oddbyte;
	register short answer;

	sum = 0;
	while (nbytes > 1) {
		sum += *ptr++;
		nbytes -= 2;
	}
	if (nbytes == 1) {
		oddbyte = 0;
		*((u_char*)&oddbyte) = *(u_char*)ptr;
		sum += oddbyte;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum = sum + (sum >> 16);
	answer = (SHORT)~sum;

	return answer;
}