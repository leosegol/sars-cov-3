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

uint16_t in_checksum(uint16_t* ptr, int nbytes);

uint16_t net_checksum_tcpudp(uint16_t length, uint16_t proto, uint8_t* addrs, uint8_t* buf);

uint8_t getDHCPtype(DHCP_header& hDHCP);

uint32_t getRequestedIP(DHCP_header& hDHCP);

uint8_t* getARPinformation(uint32_t ipv4); // ty daniel

bool checkForWantedSite(char* cSite, AddressInfo& info);

std::string fromDNSnameToDomain(char* DNSname);

uint32_t hexToIP(DWORD hexIP);

uint32_t in_fcs(uint8_t* packet, int nbytes);