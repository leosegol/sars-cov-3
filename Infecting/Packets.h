#pragma once
#include "Headers.h"
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
	uint8_t htype;
	uint8_t byteMac[8];
	uint32_t Index;
};

void CreateDHCPDiscoverPacket(char* raw_packet, AddressInfo& info);
