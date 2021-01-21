#include "Spoofing.h"
#include "Utils.h"
#include <thread>
int main()
{
	WSADATA wsock;
	WSAStartup(MAKEWORD(2, 2), &wsock);

	AddressInfo info{};
	getAddrInfo(&info);

#if _DEBUG			
	std::cout <<
		info.hostName
		<< "\n" <<
		info.ipv4
		<< "\n" <<				// print the info of the network
		info.netmask
		<< "\n" <<
		info.broadcast
	<< std::endl;
#endif

	//std::thread starvation(startDHCPStarvation, info);	// start a thread for filling the address's pool
	startDHCPSpoofing(info);												// of the dhcp server
}