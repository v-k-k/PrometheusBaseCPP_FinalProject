#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <ctime>
#include <iomanip>
#include "../exceptions/customEx.h"
#include "../tools/tools.h"

#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
typedef SOCKET Socket;
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include  <netdb.h>
typedef int Socket;
#endif

/// @brief Represents a client on the server side with it's ip, port and id properties.
class Client {
    private:
        int port;
        std::string ip;
        int id;
        static int objCounter;
        static const std::vector<std::string> serializationDelimiters;
        Client(int id, int port, std::string addr);

    public:
        /// @brief Constructs a Client object.
        /// @param port The client's port.
        /// @param addr The client's ip address.
        Client(int port, std::string addr);
        ~Client();
        std::string serialize() const;
        static Client* deserialize(const std::string str);
        void log(LogLevel level, std::string msg);
        int getId() const;

};

Socket initClientSocket();

sockaddr_in createAddress(u_short port);

void connectToServer(Socket clientSocket, sockaddr_in addr);

void receiveFromServer(Socket clientSocket, char* buffer, int buffer_size);

void closeClientSocket(Socket clientSocket, std::string errMsg = "", bool gotErr = false);

#endif