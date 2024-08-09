#include "client.h"

int Client::objCounter = 0;

/// @brief Implementation of the Client private constructor.
Client::Client(int id, int port, std::string addr) {
    this->id = id;
    this->port = port;
    this->ip = addr;
    objCounter++;
}

/// @brief Implementation of the Client public constructor.
Client::Client(int port, std::string addr) : id(++objCounter) {
    this->port = port;
    this->ip = addr;
}

/// @brief Implementation of the Client destructor
Client::~Client() = default;

const std::vector<std::string> Client::serializationDelimiters = {" <<", " : ", ">> "};

std::string Client::serialize() const {
    std::string selfRepr = std::to_string(id) + 
                           Client::serializationDelimiters[0] + 
                           ip + 
                           Client::serializationDelimiters[1] + 
                           std::to_string(port) + 
                           Client::serializationDelimiters[2];
    return selfRepr;
}

Client* Client::deserialize(const std::string str) {
    std::vector<std::string> respTokens = split(str, Client::serializationDelimiters);
    int id = std::stoi(respTokens[0]);
    int port = std::stoi(respTokens[2]);
    std::string addr = respTokens[1];
    return new Client(id, port, addr);
}

/// @brief Implementation of the log method
void Client::log(LogLevel level, std::string msg){
    static const char* level_names[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
    if (level >= INFO) // Adjust this threshold as needed
        std::cerr << "[[ " << level_names[level] << " ]]\t" << "C_" << serialize() << "[ " << getCurrentTimeString() << " ]: " << msg << std::endl;
}

int Client::getId() const {
    return id;
}

Socket initClientSocket() {
    Socket clientSocket;

#ifdef _WIN32
    
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        throw CustomException("WSAStartup failed: " + result);
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        WSACleanup();
        throw CustomException("Socket startup failed: " + WSAGetLastError());
    }

#else

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        throw CustomException("socket failed");
    }

#endif

    return clientSocket;
}

sockaddr_in createAddress(u_short port) {
    sockaddr_in addr;
#ifdef _WIN32	
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
#else
	struct hostent *hostinfo = gethostbyname("127.0.0.1");
    if (hostinfo == NULL) 
		throw CustomException("Error resolving hostname");
	memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    memcpy(&addr.sin_addr, hostinfo->h_addr, hostinfo->h_length);
#endif
    addr.sin_port = htons(port);
    return addr;
}

void closeClientSocket(Socket clientSocket, std::string errMsg, bool gotErr) {
#ifdef _WIN32

    closesocket(clientSocket);
    WSACleanup();
    errMsg = errMsg + " failed" + std::to_string(WSAGetLastError());

#else

    close(clientSocket);
    errMsg = errMsg + " failed";

#endif

    if (gotErr) 
        throw CustomException(errMsg);
}

void connectToServer(Socket clientSocket, sockaddr_in addr){
    if (connect(clientSocket, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        closeClientSocket(clientSocket, "Connection", true);
    }
}

void receiveFromServer(Socket clientSocket, char* buffer, int buffer_size){
    memset(buffer, 0, buffer_size); // Clear the buffer
    int recvResult = recv(clientSocket, buffer, buffer_size - 1, 0); // Leave space for null terminator
    if (recvResult > 0){
        buffer[recvResult] = '\0'; // Null-terminate the received data
        std::cout << buffer << std::endl;
    }
    else
        closeClientSocket(clientSocket, "Receive", true);
}
