#include <limits>
#include <ctime>
#include "server/server.h"
#include "client/client.h"


int main() {
    int userDecision;
    int intPort;
        
    std::cout << "WELCOME TO SIMPLE SOCKET APP!" << std::endl;
    std::cout << "Chose which type of socket you want to use:" << std::endl;
    std::cout << "1. SERVER" << std::endl;
    std::cout << "2. CLIENT" << std::endl;

    std::cin >> userDecision;

    if (userDecision == 1){
        std::cout << "Enter the port number: ";
        std::cin >> intPort;

        // Checking for overflow
        if (intPort < 0 || intPort > std::numeric_limits<u_short>::max()) {
            std::cerr << "Incorrect port number to convrt in u_short." << std::endl;
            return 1;
        }

        u_short usPort = static_cast<u_short>(intPort);

        Socket listenSocket = initListenSocket();

        sockaddr_in serverAddr = createServerAddress(usPort);

        bindListenSocket(listenSocket, serverAddr);

        startListeninig(listenSocket, serverAddr, usPort);


        while (true) {
            Socket clientSocket;
            std::string errMsg;
            if (acceptClientFailed(listenSocket, &clientSocket, &errMsg)) {
                std::cout << errMsg << std::endl;
                continue;
            }

            auto [client_ip, client_port] = getClientIpPort(clientSocket);

            Client* clt = new Client(1, client_port, client_ip);
            clt->log("We have " + std::to_string(clt->id) + " client");

            responseHello(clientSocket);


            arrayProduct(clientSocket);
            
            clt->log("hole");

            // _WIN32 closesocket(clientSocket);
            //  POSIX close(clientSocket);
        }

        closeListenSocket(listenSocket);
    }

    
    return 0;
}


