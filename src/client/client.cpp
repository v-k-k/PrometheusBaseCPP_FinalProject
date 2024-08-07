#include <ctime>
#include "client.h"

int Client::objCounter = 0;

/// @brief Implementation of the Client constructor.
Client::Client(int port, std::string addr) : id(++objCounter) {
    this->port = port;
    this->ip = addr;
}

/// @brief Implementation of the Client destructor
Client::~Client() = default;

/// @brief Implementation of the log method
void Client::log(std::string msg) {       
    // Get the current time as a time_t object
    time_t now = time(nullptr);

    // Convert to local time
    tm* local_time = localtime(&now);

    // Format the time as a string
    char t_stamp[80];
    strftime(t_stamp, sizeof(t_stamp), "%Y-%m-%d %H:%M:%S", local_time);

    std::cout << "C_" << id << " <<" << ip << " : " << port << ">> " << "[ " << t_stamp << " ]: " << msg << std::endl;
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
    /*struct hostent *hostinfo = gethostbyname("127.0.0.1");
        if (hostinfo == NULL) {
            perror("Error resolving hostname");
            exit(1);
        }

        // Fill server address structure
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        memcpy(&addr.sin_addr, hostinfo->h_addr, hostinfo->h_length);
        addr.sin_port = htons(usPort);*/
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
    if (recvResult > 0) {
        buffer[recvResult] = '\0'; // Null-terminate the received data
        std::cout << buffer << std::endl;
    }
    else {
        closeClientSocket(clientSocket, "Receive", true);
    }
}
