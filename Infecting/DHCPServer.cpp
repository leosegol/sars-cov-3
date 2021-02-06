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

	getAddrInfo(&info);
	

	startSpoofing((LPVOID)&info);

	// DHCP spoofing
	/*
	hThread = CreateThread(
		NULL,
		0,
		startSpoofing,
		(LPVOID)&info,
		0,
		&hThreadID
	);

	WaitForMultipleObjects(1, &hThread, TRUE, INFINITE);
	*/
}