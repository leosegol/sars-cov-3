#pragma once
#include "Packets.h"

void sendDiscoverPacket(AddressInfo& info);
void recvDHCPPsackets(AddressInfo& info);

void startDHCPStarvation(AddressInfo& info);
void startDHCPSpoofing(AddressInfo& info);