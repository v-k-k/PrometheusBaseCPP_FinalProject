#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>

/// @brief Represents a client on the server side with it's ip, port and id properties.
class Client {
    private:
        int port;
        std::string ip;
        int id;
        static int objCounter;

    public:
        /// @brief Constructs a Client object.
        /// @param port The client's port.
        /// @param addr The client's ip address.
        Client(int port, std::string addr);
        ~Client();
        void log(std::string msg);
        int getId() const;

};

#endif