#include "tools.h"

void showMainMenu(){
    std::cout << "WELCOME TO SIMPLE SOCKET APP!" << std::endl;
    std::cout << "Chose which type of socket you want to use:" << std::endl;
    std::cout << "1. SERVER" << std::endl;
    std::cout << "2. CLIENT" << std::endl;
}

std::pair<std::string, std::function<void(std::function<void(std::string)>, Socket)>> userMadeDecision(int decIdx) {
    switch (decIdx) {
        case 1:
            return {"You choose calculate array sum. Input the array members (press `=` to calculate):\r\n",
                    [=](std::function<void(std::string)> callbackLogger, Socket clientSocket) {
                        collectAndProcess(callbackLogger, clientSocket, Operation::add);
                    }};
        case 2:
            return {"You choose calculate array product. Input the array members (press `=` to calculate):\r\n",
                    [=](std::function<void(std::string)> callbackLogger, Socket clientSocket) {
                        collectAndProcess(callbackLogger, clientSocket, Operation::multiply);
                    }};
        case 3:
            return {"You choose exit.\r\n", closeClientConnection};
        default:
            throw CustomException("Invalid input");
    }
}
