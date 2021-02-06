#pragma once
#include <winsock2.h>
#include <windows.h>
#include <stdint.h>

struct AddressInfo
{
	uint8_t hostName[255];
	uint8_t ipv4[16];
	uint8_t noIP[16];
	uint8_t netmask[16];
	uint8_t broadcast[16];
	uint8_t gateWay[16];
	uint8_t domainName[4];
	uint8_t htype;
	uint8_t byteMac[8];
	uint8_t AdaptersName[260];
	uint32_t Index;
};

void createDHCPDiscoverPacket(char* raw_packet, AddressInfo& info);
void createDHCPOfferPacket(char* raw_packet, void* pDHCP, void* pIP, AddressInfo& info);
void createDHCPackPacket(char* raw_packet, void* pDHCP, void* pIP, AddressInfo& info);
size_t createDNSResponsePacket(char* raw_packet, char* qDNS, AddressInfo& info);