#include "Utils.h"
#include <Windows.h>

void printVictims(uint32_t* address)
{
	std::string nextline;
	int i = 0;
	in_addr addr;

	nextline = "List of victims:\n";
	while (address[i] != NULL)
	{
		addr.s_addr = address[i];
		std::cout << "[" << ++i << "] " << inet_ntoa(addr) << '\n';
	}
	std::cout << std::endl;
}

uint32_t chooseVictim(uint32_t* address)
{
	int victimIndex;
	do
	{
		std::cout << "Enter victim by number> ";
		std::cin >> victimIndex;
		std::cout << std::endl;
	} while (address[victimIndex] == NULL);
	return address[victimIndex];
}
