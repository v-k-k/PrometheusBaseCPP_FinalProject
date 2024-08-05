#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>

/// @brief Represents a client with it's ip, port and id properties.
class Client {
    private:
        int port;
        std::string ip;

    public:
        /// @brief Constructs a Client object.
        /// @param id The client's id.
        /// @param port The client's port.
        /// @param addr The client's ip address.
        Client(int id, int port, std::string addr);
        ~Client();
        int id;
        void log(std::string msg);

};

#endif