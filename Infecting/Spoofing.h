#pragma once
#include "Packets.h"
#include "Headers.h"

void sendDiscoverPacket(AddressInfo& info);
void recvDHCPPsackets(AddressInfo& info);
void sendOfferPacket(DHCP_header& hHeader, AddressInfo& info);


void startDHCPStarvation(AddressInfo& info);
void startDHCPSpoofing(AddressInfo& info);