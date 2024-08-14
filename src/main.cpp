// #include <limits> /* Uncomment in case if overflow check required */
#include <ctime>
#include <thread>
#include "server/server.h"
#include "client/client.h"
#include "tools/tools.h"


void handleClient(Socket clientSocket) {
    int userDecision;
    auto [client_ip, client_port] = getClientIpPort(clientSocket);
    Client* clt = new Client(client_port, client_ip);
    clt->log(INFO, "We have " + std::to_string(clt->getId()) + " client");

    /*
     * Uncomment `responseHello` function in case of using third-party socket client like Putty etc.
     */
    // responseHello([clt](LogLevel level, std::string msg) { clt->log(INFO, msg); }, clientSocket);

    // Greet te client
    std::string greetMsg = "WELCOME TO SIMPLE SOCKET SERVER!!!\r\n" + clt->serialize() + "\r\n";
    send(clientSocket, greetMsg.c_str(), greetMsg.length(), 0);

    while (true) {
        // Provide the server menu
        respondWithText(SERVER_MENU, [clt](LogLevel level, std::string msg) { clt->log(INFO, msg); }, clientSocket);

        userDecision = handleClientDecision([clt](LogLevel level, std::string msg) { clt->log(INFO, msg); }, clientSocket);

        // calculate
        auto chosenFunc = userMadeDecision(userDecision);
        respondWithText(chosenFunc.first, [clt](LogLevel level, std::string msg) { clt->log(INFO, msg); }, clientSocket);
        chosenFunc.second([clt](LogLevel level, std::string msg) { clt->log(INFO, msg); }, clientSocket);
    }

    // Handle client disconnection or error
    delete clt;
    closeClientSocket(clientSocket);
}


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

    if (userDecision == 1){ // SERVER
        Socket listenSocket = initListenSocket();
        sockaddr_in serverAddr = createServerAddress(usPort);

        bindListenSocket(listenSocket, serverAddr);
        startListeninig(listenSocket, serverAddr, usPort);

        // Create the vector of client-handling threads
        std::vector<std::thread> threads;
        while (true) {
            Socket clientSocket;
            std::string errMsg;
            if (acceptClientFailed(listenSocket, &clientSocket, &errMsg)) {
                std::cout << errMsg << std::endl;
                continue;
            }
            threads.emplace_back(handleClient, std::move(clientSocket));
        }

        // Wait for all threads to finish
        for (auto& t : threads) t.join();

        closeListenSocket(listenSocket);
    }
    else if (userDecision == 2){ // CLIENT
        Socket clientSocket = initClientSocket();
        sockaddr_in addr = createAddress(usPort);
        connectToServer(clientSocket, addr);

        char buffer[BUFFER_SIZE];
        int recvResult;

        // Receive welcome message
        receiveFromServer(clientSocket, buffer, BUFFER_SIZE);

        std::string clientInWelcomeMessage = split(buffer, { "\r\n" })[1];
        Client* clt = Client::deserialize(clientInWelcomeMessage);
        clt->log(WARNING, "Client object has been created");

        // Receive menu options
        receiveFromServer(clientSocket, buffer, BUFFER_SIZE);
        clt->log(DEBUG, "Received " + std::to_string(sizeof(buffer)) + " bytes from server");

        std::string choice;

        while (true) {
            // Choose operation
            std::cin >> choice;
            choice += "\r\n";
            send(clientSocket, choice.c_str(), choice.length(), 0);
            clt->log(WARNING, "You've chosen " + choice);

            // Receive sever options
            receiveFromServer(clientSocket, buffer, BUFFER_SIZE);
            clt->log(DEBUG, "Received " + std::to_string(sizeof(buffer)) + " bytes from server");

            if (choice == "3\r\n") break;

            if (choice == "1\r\n" || choice == "2\r\n") {   
                std::string inputElement;         
                while (true) {
                    std::cin >> inputElement;
                    inputElement += "\r\n";
                    send(clientSocket, inputElement.c_str(), inputElement.length(), 0);
                    clt->log(DEBUG, "You've sent " + inputElement);
                    if (inputElement == "=\r\n") break;
                }

                // Receive result
                receiveFromServer(clientSocket, buffer, BUFFER_SIZE);
                clt->log(DEBUG, "Received " + std::to_string(sizeof(buffer)) + " bytes from server");
            }
        }
        // Receive goodbye
        receiveFromServer(clientSocket, buffer, BUFFER_SIZE);
        clt->log(DEBUG, "Goodbye message " + std::to_string(sizeof(buffer)) + " bytes from server");

        closeClientSocket(clientSocket);
        clt->log(WARNING, "Client socket has been closed");
    }
        
    return 0;
}


