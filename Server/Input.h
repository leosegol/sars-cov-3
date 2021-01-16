#include<iostream>
#include<string>
#include<map>
#include<WinSock2.h>

class Input
{
public:
	std::map<std::string, SOCKET> list;
	std::string command;
	std::string client;
	static Input input;
	static std::string RET;

private:
	Input(); // Using singleton to create the screen

public:
	std::string In();
	void Out(std::string);
	SOCKET ChooseClient();
	void AddClient(sockaddr_in, SOCKET);
};


