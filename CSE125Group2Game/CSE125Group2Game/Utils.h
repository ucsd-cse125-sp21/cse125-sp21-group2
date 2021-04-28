#pragma once
#include <iostream>
#include <string>

// Util macros
#define CRASH(msg) PrintCrash(msg, __FILE__, __LINE__)
#define ASSET(name) ("Assets/" + std::string(name))

void PrintCrash(const std::string& msg, const std::string& file, int line);
