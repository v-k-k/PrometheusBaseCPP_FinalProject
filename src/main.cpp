// #include <limits>
#include <ctime>
#include "server/server.h"
#include "client/client.h"
#include "tools/tools.h"


int main() {
    int userDecision;
    int intPort;
        
    showMainMenu();
    std::cin >> userDecision;

    std::cout << "Enter the port number: ";
    std::cin >> intPort;

    /*
     * Unnecessary overflow check
     *  
     * if (intPort < 0 || intPort > std::numeric_limits<u_short>::max()) {
     *     std::cerr << "Incorrect port number to convrt in u_short." << std::endl;
     *     return 1;
     * }
     */

    u_short usPort = static_cast<u_short>(intPort);

    if (userDecision == 1){
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
             * Uncomment `responseHello` function in case of using third-party socket client like Putty etc.
             */
            // responseHello(std::bind(&Client::log, clt, std::placeholders::_1), clientSocket);
            send(clientSocket, "WELCOME TO SIMPLE SOCKET SERVER!!!\r\n", 7, 0);

            while (true) {
                respondWithText(SERVER_MENU, std::bind(&Client::log, clt, std::placeholders::_1), clientSocket);
                
                userDecision = handleClientDecision(std::bind(&Client::log, clt, std::placeholders::_1), clientSocket);
                clt->log("[[ WARNING ]]: " + std::to_string(userDecision));

                auto chosenFunc = userMadeDecision(userDecision);
                respondWithText(chosenFunc.first, std::bind(&Client::log, clt, std::placeholders::_1), clientSocket);
                chosenFunc.second(std::bind(&Client::log, clt, std::placeholders::_1), clientSocket);
            }
        }

        closeListenSocket(listenSocket);
    }
    else if (userDecision == 2){
        Socket clientSocket = initClientSocket();
        sockaddr_in addr = createAddress(usPort);
        connectToServer(clientSocket, addr);

        char buffer[BUFFER_SIZE];
        int recvResult;/**/

        // Receive welcome message
        receiveFromServer(clientSocket, buffer, BUFFER_SIZE);

        // Receive menu options
        receiveFromServer(clientSocket, buffer, BUFFER_SIZE);

        std::string choice;

        while (true) {
            // Choose operation
            std::cin >> choice;
            choice += "\r\n";
            send(clientSocket, choice.c_str(), choice.length(), 0);

            // Receive sever options
            receiveFromServer(clientSocket, buffer, BUFFER_SIZE);

            if (choice == "3\r\n") break;

            if (choice == "1\r\n" || choice == "2\r\n") {   
                std::string inputElement;         
                while (true) {
                    std::cin >> inputElement;
                    inputElement += "\r\n";
                    send(clientSocket, inputElement.c_str(), inputElement.length(), 0);
                    std::cout << "SENT: " << inputElement << std::endl;
                    if (inputElement == "=\r\n") break;
                }

                // Receive result
                receiveFromServer(clientSocket, buffer, BUFFER_SIZE);
            }
        }
        // Receive goodbye
        receiveFromServer(clientSocket, buffer, BUFFER_SIZE);

        closeClientSocket(clientSocket);
    }
        
    return 0;
}


