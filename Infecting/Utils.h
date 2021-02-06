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

uint16_t in_checksum(unsigned short* ptr, int nbytes);

uint8_t getDHCPtype(DHCP_header& hDHCP);

uint32_t getRequestedIP(DHCP_header& hDHCP);

uint8_t* getARPinformation(uint32_t ipv4, AddressInfo& info); // ty daniel