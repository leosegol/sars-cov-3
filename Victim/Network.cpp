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
    sockaddr_in sockinfo;
    sockaddr_in mastersinfo;
    SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == INVALID_SOCKET)
        std::cout << "Socket error <" << WSAGetLastError() << ">" << std::endl;
    int error;

    std::string message, response;
    char* buf = new char[65536];
    int fromlen;

    sockinfo.sin_addr.s_addr = getPrivateIP();
    sockinfo.sin_port = htons(667);
    sockinfo.sin_family = AF_INET;

    fromlen = sizeof(sockinfo);

    error = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&error, sizeof(error))<0)
        std::cout << "Sockopt error <" << WSAGetLastError() << ">" << std::endl;

    if(bind(s, (sockaddr*)&sockinfo, sizeof sockinfo) < 0)
        std::cout << "Bind error <" << WSAGetLastError() << ">" << std::endl;

    /*fixed messages for my "protocol"*/
    message = "What is your IP?";
    response = "My IP";

    /*wait for the master to ask me to connect*/
    do {
        ZeroMemory(buf, 65536);
        error = recvfrom(s, buf, 65536, 0, (sockaddr*)&sockinfo, &fromlen);
        std::cout << buf;
        if (error < 0)
        {
            std::cout << "Recieve error <" << WSAGetLastError() << ">" << std::endl;
            goto errorLable;
        }
    } while (!std::string(buf)._Equal(message));

    error = sendto(s, response.c_str(), response.size(), 0, (sockaddr*)&sockinfo, sizeof(sockinfo));
    if (error < 0)
        std::cout << "Send error <" << WSAGetLastError() << ">" << std::endl;

errorLable:
    delete[] buf;
    closesocket(s);
    return sockinfo.sin_addr.s_addr;
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
