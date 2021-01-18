#pragma once
#include "Headers.h"

#include <winsock2.h>
#include <Windows.h>

struct AddressInfo
{
	char hostName[255];
	char ipv4[16];
	char netmask[16];
	char broadcast[16];
};

void CreateDHCPDiscoverPacket(char* raw_packet, AddressInfo& info);
