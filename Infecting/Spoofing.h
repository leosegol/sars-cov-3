#pragma once
#include "Packets.h"
#include "Headers.h"

int sendDiscoverPacket(AddressInfo& info);
int recvDHCPPsackets(AddressInfo& info);
int sendOfferPacket(DHCP_header& rDHCP, IP_header& rIP, AddressInfo& info);


DWORD WINAPI startDHCPStarvation(LPVOID info);
DWORD WINAPI startDHCPSpoofing(LPVOID info);