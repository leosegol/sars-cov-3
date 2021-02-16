#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define HAVE_REMOTE
#define WPCAP

#include "Headers.h"
#include "Packets.h"
#include "Utils.h"

#include <algorithm>

#include <pcap.h>
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "packet.lib")

#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "IPHLPAPI.lib")

#include <string>

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
	memcpy((char*)info->DNS_IP, "8.8.8.8", 16);
	memcpy((char*)info->netmask, address->IpMask.String, 16);
	memcpy((char*)info->broadcast, inet_ntoa(paddr), 16);
	memcpy((char*)info->domainName, pFixedInfo->DomainName, 4);
	free(pAdapterInfo);
}

bool checkForDHCP(DHCP_header& hDHCP)
{
	try
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
	catch (...)
	{
		return false;
	}
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

uint8_t getDHCPtype(DHCP_header& hDHCP)
{
	for (int i = 0; i < sizeof DHCP_header::opt; i++)
		if (hDHCP.opt[i] == 53)
			return hDHCP.opt[i + 2];
}

uint32_t getRequestedIP(DHCP_header& hDHCP)
{
	int i;
	for (i = 0; i < sizeof(DHCP_header::opt); i++)
	{
		if (hDHCP.opt[i] == 50)
			return *(uint32_t*)&hDHCP.opt[i + 2];
	}
	return 0;
}

uint8_t* getARPinformation(uint32_t ipv4)
{
	DWORD i;
	PMIB_IPNETTABLE pIpNetTable = NULL;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;
	DWORD dwResult;

	dwResult = GetIpNetTable(NULL, &dwSize, 0);
	if (dwResult == ERROR_INSUFFICIENT_BUFFER)
		pIpNetTable = (MIB_IPNETTABLE*)malloc(dwSize);

	/* Now that we know the size, lets use GetIpNetTable() */
	if ((dwRetVal = GetIpNetTable(pIpNetTable, &dwSize, 0)) == NO_ERROR)
		if (pIpNetTable->dwNumEntries > 0)
			for (i = 0; i < pIpNetTable->dwNumEntries; i++)
			{
				//std::cout << pIpNetTable->table[i].dwAddr << ',';
				//printHex((char*)pIpNetTable->table[i].bPhysAddr, 6);
				if (pIpNetTable->table[i].dwAddr == ipv4
					|| hexToIP(pIpNetTable->table[i].dwAddr) == ipv4)
					return pIpNetTable->table[i].bPhysAddr;
			}
	free(pIpNetTable);
	return 0;
}

bool checkForWantedSite(char* cSite, AddressInfo& info)
{
	std::string domain = fromDNSnameToDomain(cSite);
	for (int i = 0; i < HIJACKED_SITES; i++)
	{
		if (domain.compare(std::string(info.DNS_sites[i])) == 0)
			return true;
	}
	return false;
}

std::string fromDNSnameToDomain(char* DNSname)
{
	std::string flat = "";
	std::string DC(DNSname);
	int nZone;
	while (DC.length())
	{
		nZone = DC[0];
		if (nZone > DC.length())
			return flat;
		flat += DC.substr(1, nZone);
		flat += '.';
		DC = DC.substr(nZone + 1);
	}
	flat = flat.substr(0, flat.length() - 1);
	return flat;
}

uint32_t hexToIP(DWORD hexIP)
{
	uint8_t* byteIP = (uint8_t*)&hexIP;
	std::string s;
	int zone;
	for (int i = 0; i < 4; i++)
	{
		zone = byteIP[i];
		s += std::to_string(static_cast<unsigned>(zone));
		s += ".";
	}
	s[s.length() - 1] = '\0';
	return inet_addr(s.c_str());
}



