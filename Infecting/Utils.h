#pragma once
#include "Packets.h"
#include "Headers.h"

#include <iostream>

void getAddrInfo(AddressInfo* info);

bool checkForDHCP(DHCP_header& hDHCP);

void printHex(char* str, size_t size);

void printIP(uint32_t byteIP);

uint32_t createRandomIP(AddressInfo& info);

const char* getDeviceName(AddressInfo& info);