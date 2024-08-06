#include <limits>
#include <ctime>
#include "server/server.h"
#include "client/client.h"
#include "tools/tools.h"


int main() {
    int userDecision;
    int intPort;
        
    showMainMenu();

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

            Client* clt = new Client(client_port, client_ip);
            clt->log("We have " + std::to_string(clt->getId()) + " client");

            /*
             * Temporarily stuff.
             * Uncomment in case of using third-party socket client like Putty etc.
             */
            responseHello(std::bind(&Client::log, clt, std::placeholders::_1), clientSocket);

            while (true) {
                respondWithText(SERVER_MENU, std::bind(&Client::log, clt, std::placeholders::_1), clientSocket);
                
                userDecision = handleClientDecision(clientSocket);

                auto chosenFunc = userMadeDecision(userDecision);
                respondWithText(chosenFunc.first, std::bind(&Client::log, clt, std::placeholders::_1), clientSocket);
                chosenFunc.second(std::bind(&Client::log, clt, std::placeholders::_1), clientSocket);
            }
        }

        closeListenSocket(listenSocket);
    }

    
    return 0;
}


