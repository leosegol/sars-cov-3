#include<iostream>
#include<WinSock2.h>
#include<windows.h>
#include<WS2tcpip.h>
#include<string>
#include<map>
#pragma comment(lib, "ws2_32.lib")
#include "socket.h"
#include "Input.h"

int TelNet(SOCKET sock)
{
	//SOCKET sock = *(SOCKET*)par;
	while (1)
	{
		std::string cmd = Input::input.In();
		if (!cmd.compare(Input::RET))
		{
			Input::input.client = "";
			break;
		}
		send(sock, cmd.c_str(), (int)cmd.size(), 0);

		int n;
		char temp[4096] = { 0 };
		std::string output = "";
		do
		{
			n = recv(sock, temp, sizeof temp, 0);
			if (n == 0)
				return 1;
			output += std::string(temp);
			memset(temp, 0, sizeof temp);
		} while (n >= 4096);
		Input::input.Out(output);
	}
	return 0;
}

DWORD WINAPI options(LPVOID lpParameter)
{
	while (1)
		TelNet(Input::input.ChooseClient());
}




int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	DWORD myThreadID;
	HANDLE myHandle = CreateThread(0, 0, options, NULL, 0, &myThreadID);
	//std::map<sockaddr_in, SOCKET> clients;
	Socket listen = Socket("0.0.0.0", 667);
	while (true)
	{
		sockaddr_in clientAddr;
		SOCKET client = listen.acccept(clientAddr);
		//clients.insert(std::pair<sockaddr_in, SOCKET>(clientAddr, client));
		Input::input.AddClient(clientAddr, client);
	}
}

