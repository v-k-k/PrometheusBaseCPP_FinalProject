#ifndef TOOLS_H
#define TOOLS_H

#include <ctime>
#include <chrono>
#include <string>
#include <iomanip>
#include "../server/server.h"
#include "enumerations.h"

#ifdef _WIN32
    #include <time.h>
#endif

#define SERVER_MENU "\r\nChose which type of calculation do you expect from server:\r\n1. CALCULATE ARRAY SUM\r\n2. CALCULATE ARRAY PRODUCT\r\n3. CLOSE CONNECTION\r\n"


void showMainMenu();

std::pair<std::string, std::function<void(std::function<void(LogLevel, std::string)>, Socket)>> userMadeDecision(int decIdx);

std::vector<std::string> split(const std::string& s, const std::vector<std::string>& delimiters);

std::string getCurrentTimeString();

#endif