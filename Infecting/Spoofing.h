#pragma once
#include "Packets.h"
#include "Headers.h"

#include <pcap.h>

int sendDiscoverPacket(AddressInfo& info);
int sendOfferPacket(DHCP_header& rDHCP, IP_header& rIP, AddressInfo& info);
int sendACKPacket(char* raw_packet, DHCP_header& rDHCP, IP_header& rIP, pcap_t* sock, AddressInfo& info);
int sendDNSResponse(char* raw_packet, char* qDNS, pcap_t* sock, AddressInfo& info);
int dnsMITM(char* qDNS, uint8_t* raw_packet, AddressInfo& info);
void setAck(AddressInfo& info);

DWORD WINAPI startDHCPStarvation(LPVOID info);
DWORD WINAPI startSpoofing(LPVOID info);
