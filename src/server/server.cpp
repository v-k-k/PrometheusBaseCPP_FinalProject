#include "server.h"

/// @brief getClientIpPort function. Returns the client's ip and port.
/// @param clientSocket The client's socket.
/// @return pair of client's ip and port.
std::pair<std::string, int> getClientIpPort(Socket clientSocket) {
    sockaddr_in clientAddr;

#ifdef _WIN32
    int addrLen = sizeof(clientAddr);
#else
    socklen_t addrLen = sizeof(clientAddr);
#endif

    if (getpeername(clientSocket, (sockaddr*)&clientAddr, &addrLen) < 0)
        throw CustomException("getpeername failed");


    char ipStr[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &(clientAddr.sin_addr), ipStr, INET_ADDRSTRLEN) == nullptr)
        throw CustomException("inet_ntop failed");

    int port = ntohs(clientAddr.sin_port);
    return { std::string(ipStr), port };
}

sockaddr_in createServerAddress(u_short port) {
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    return serverAddr;
}

std::vector<int> collectIntVector(std::function<void(std::string)> callbackLogger, Socket clientSocket) {
    std::vector<int> values;
    std::string buffer;
    bool done = false;

    while (!done) {
        char data[BUFFER_SIZE];
        int bytes_received = recv(clientSocket, data, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            break;
        }
        callbackLogger("[[ INFO ]]: RECEIVED VALUES\r\n" + std::string(buffer));

        data[bytes_received] = '\0';
        buffer += data;

        if (buffer.find("=\r\n") != std::string::npos) {
            done = true;
            buffer.erase(remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
        }
    }
    std::istringstream iss(buffer);
    int num;
    while (iss >> num) {
        values.push_back(num);
    }
    return values;
}

void collectAndProcess(std::function<void(std::string)> callbackLogger, Socket clientSocket, int perationIdx) {
    std::vector<int> values = collectIntVector(callbackLogger, clientSocket);
    int result = 0;
    std::string finished = "sum: ";
    switch (perationIdx)
    {
    case 1:
        result = 1;
        for (int v : values) result *= v;
        finished = "product: ";
        break;
    
    default:
        for (int v : values) result += v;
        break;
    }

    std::string message = "Calculated " + finished + std::to_string(result);
    callbackLogger(message);
    send(clientSocket, message.c_str(), message.length(), 0);
}

void responseHello(std::function<void(std::string)> callbackLogger, Socket clientSocket) {
    char buffer[BUFFER_SIZE];
    int bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytesReceived] = '\0'; // Null-terminate the received data
        std::cout << "Received: " << buffer << std::endl;

        // Send "Hello" as a response
        send(clientSocket, "Hello\r\n", 7, 0);
        callbackLogger("Hello\r\n");
        break;
    }
}

void respondWithText(std::string msg, std::function<void(std::string)> callbackLogger, Socket clientSocket) {
    send(clientSocket, msg.c_str(), msg.length(), 0);
    callbackLogger("Respond with " + std::to_string(msg.length()) + " bytes");
}

Socket initListenSocket() {
    Socket listenSocket;

#ifdef _WIN32
    
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        throw CustomException("WSAStartup failed: " + result);
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        WSACleanup();
        throw CustomException("Socket startup failed: " + WSAGetLastError());
    }

#else

    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == -1) {
        throw CustomException("socket failed");
    }

#endif

    return listenSocket;
}

void closeListenSocket(Socket listenSocket, std::string errMsg, bool gotErr) {
#ifdef _WIN32

    closesocket(listenSocket);
    WSACleanup();
    errMsg = errMsg + " failed" + std::to_string(WSAGetLastError());

#else

    close(listenSocket);
    errMsg = errMsg + " failed";

#endif

    if (gotErr) 
        throw CustomException(errMsg);
}

void bindListenSocket(Socket listenSocket, sockaddr_in serverAddr) {
    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        closeListenSocket(listenSocket, "binding", true);
    }
}

void startListeninig(Socket listenSocket, sockaddr_in serverAddr, u_short port) {
    if (listen(listenSocket, SOMAXCONN) == -1) {
        closeListenSocket(listenSocket, "listening", true);
    }
    std::cout << "Server listening on port " << port << std::endl;
}

bool acceptClientFailed(Socket listenSocket, Socket* clientSocket, std::string* errMsg) {

#ifdef _WIN32

    *clientSocket = accept(listenSocket, NULL, NULL);
    *errMsg = "Client was not accepted: " + WSAGetLastError();
    return *clientSocket == INVALID_SOCKET;

#else

    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    *clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientLen);
    *errMsg = "Client was not accepted";
    return *clientSocket == -1;

#endif
}

int handleClientDecision(std::function<void(std::string)> callbackLogger, Socket clientSocket) {
    char buffer[BUFFER_SIZE];
    int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
    if (bytesReceived <= 0)
        throw CustomException("No client input");

    buffer[bytesReceived] = '\0';
    callbackLogger("[[ DEBUG ]]: " + std::string(buffer));
    int clientChoice = atoi(buffer);
    return clientChoice;
}

void closeClientConnection(std::function<void(std::string)> callbackLogger, Socket clientSocket){
    callbackLogger("Closing the client connection...");
    char* byeBye = "\nBye...\n";
    send(clientSocket, byeBye, sizeof(byeBye), 0);

#ifdef _WIN32

    closesocket(clientSocket);

#else

    close(clientSocket);

#endif
}
