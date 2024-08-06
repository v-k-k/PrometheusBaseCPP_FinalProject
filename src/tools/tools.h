#ifndef TOOLS_H
#define TOOLS_H

#include "../server/server.h"

#define SERVER_MENU "\r\nChose which type of calculation you expect from server:\r\n1. CALCULATE ARRAY SUM\r\n2. CALCULATE ARRAY PRODUCT\r\n3. CLOSE CONNECTION\r\n"

void showMainMenu();

std::pair<std::string, std::function<void(std::function<void(std::string)>, Socket)>> userMadeDecision(int decIdx);

enum Operation{
    add,
    multiply
};

#endif