#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <utility>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <map>
#include "../exceptions/customEx.h"
#include "../tools/enumerations.h"

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

std::pair<std::string, int> getClientIpPort(Socket clientSocket);

sockaddr_in createServerAddress(u_short port);

void collectAndProcess(std::function<void(LogLevel, std::string)> callbackLogger, Socket clientSocket, int perationIdx);

void responseHello(std::function<void(LogLevel, std::string)> callbackLogger, Socket clientSocket);

Socket initListenSocket();

void closeListenSocket(Socket listenSocket, std::string errMsg = "", bool gotErr = false);

void bindListenSocket(Socket listenSocket, sockaddr_in serverAddr);

void startListeninig(Socket listenSocket, sockaddr_in serverAddr, u_short port);

bool acceptClientFailed(Socket listenSocket, Socket* clientSocket, std::string* errMsg);

int handleClientDecision(std::function<void(LogLevel, std::string)> callbackLogger, Socket clientSocket);

void respondWithText(std::string msg, std::function<void(LogLevel, std::string)> callbackLogger, Socket clientSocket);

void closeClientConnection(std::function<void(LogLevel, std::string)> callbackLogger, Socket clientSocket);

#endif
