#pragma once
#include "Packets.h"

#include <iostream>

void getAddrInfo(AddressInfo* info);

bool checkForDHCP(DHCP_header& hDHCP);