#include "Packets.h"
#include <iostream>
#include <winsock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#pragma pack(1)
#pragma comment(lib, "Ws2_32.lib")

void createDiscoverPacket(char* packet, size_t pHeader);
void createIPv4Packet(char* packet, size_t pHeader, uint16_t total_size, PCSTR& ip);
void createUDPpacket(char* packet, size_t pHeader, uint16_t src_port, uint16_t dst_port, uint16_t p_size);
void createEthernetPacket(char* packet, size_t pHeader, uint8_t* dst_mac, uint8_t* src_mac);

void sendDHCPpacket()
{
	WSADATA wsock;
	WSAStartup(MAKEWORD(2, 2), &wsock);

	SOCKET s;
	sockaddr_in dst;

	s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW); //Create a RAW socket
	if(s == SOCKET_ERROR)
		std::cout << WSAGetLastError();
	int optval = 1;
	setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char*)&optval, sizeof optval);
	dst.sin_family = AF_INET;
	dst.sin_port = htons(67);
	inet_pton(AF_INET, "10.0.0.255", &dst.sin_addr.s_addr);
	char* raw_packet = new char[65536];

	PCSTR ip = "10.0.0.255";
	createIPv4Packet(raw_packet, 0, sizeof(IP_header) + sizeof(DHCP_header) + sizeof(UDP_header), ip);
	createUDPpacket(raw_packet, sizeof(IP_header), 68, 67, sizeof(UDP_header) +sizeof(DHCP_header));
	createDiscoverPacket(raw_packet, sizeof(IP_header) + sizeof(UDP_header));


	sendto(s, raw_packet, sizeof(IP_header) + sizeof(UDP_header) + sizeof(DHCP_header) , 0, (sockaddr*)&dst, sizeof(dst));
	delete[] raw_packet;
}

int main()
{
	sendDHCPpacket();
}