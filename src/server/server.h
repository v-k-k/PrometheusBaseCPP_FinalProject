#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <exception>
#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <map>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET Socket;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int Socket;
#endif

#define BUFFER_SIZE 1024

class CustomException : public std::exception {
public:
    explicit CustomException(const std::string& message) : message_(message) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }

private:
    std::string message_;

};

std::pair<std::string, int> getClientIpPort(Socket clientSocket);

sockaddr_in createServerAddress(u_short port);

void collectAndProcess(std::function<void(std::string)> callbackLogger, Socket clientSocket, int perationIdx);

void responseHello(std::function<void(std::string)> callbackLogger, Socket clientSocket);

Socket initListenSocket();

void closeListenSocket(Socket listenSocket, std::string errMsg = "", bool gotErr = false);

void bindListenSocket(Socket listenSocket, sockaddr_in serverAddr);

void startListeninig(Socket listenSocket, sockaddr_in serverAddr, u_short port);

bool acceptClientFailed(Socket listenSocket, Socket* clientSocket, std::string* errMsg);

int handleClientDecision(Socket clientSocket);

void respondWithText(std::string msg, std::function<void(std::string)> callbackLogger, Socket clientSocket);

void closeClientConnection(std::function<void(std::string)> callbackLogger, Socket clientSocket);

#endif
