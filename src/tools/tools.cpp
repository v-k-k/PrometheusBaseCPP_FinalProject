#include "tools.h"

void showMainMenu(){
    std::cout << "WELCOME TO SIMPLE SOCKET APP!" << std::endl;
    std::cout << "Chose which type of socket you want to use:" << std::endl;
    std::cout << "1. SERVER" << std::endl;
    std::cout << "2. CLIENT" << std::endl;
}

std::string getCurrentTimeString() {
    // Get current time as a time point
    auto now = std::chrono::system_clock::now();

    // Convert to system time
    time_t tt = std::chrono::system_clock::to_time_t(now);

    // Convert to local time
    struct tm local_time;

#ifdef _WIN32
    localtime_s(&local_time, &tt);
#else
    localtime_r(&tt, &local_time);
#endif

    // Format the time as a string
    std::stringstream ss;
    ss << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::pair<std::string, std::function<void(std::function<void(LogLevel, std::string)>, Socket)>> userMadeDecision(int decIdx) {
    switch (decIdx) {
        case 1:
            return {"You choose calculate array sum. Input the array members (press `=` to calculate):\r\n",
                    [](std::function<void(LogLevel, std::string)> callbackLogger, Socket socket) {
                     collectAndProcess(callbackLogger, socket, Operation::add);} };
        case 2:
            return {"You choose calculate array product. Input the array members (press `=` to calculate):\r\n",
                    [](std::function<void(LogLevel, std::string)> callbackLogger, Socket socket) {
                     collectAndProcess(callbackLogger, socket, Operation::multiply);} };
        case 3:
            return {"You choose exit.\r\n", 
                    [](std::function<void(LogLevel, std::string)> callbackLogger, Socket socket) {
                     closeClientConnection(callbackLogger, socket);} };
        default:
            throw CustomException("Invalid input");
    }
}

std::vector<std::string> split(const std::string& s, const std::vector<std::string>& delimiters) {
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;

    while (start < s.length()) {
        size_t min_pos = std::string::npos;
        for (const std::string& delim : delimiters) {
            size_t pos = s.find(delim, start);
            if (pos != std::string::npos && pos < min_pos) min_pos = pos;
        }
        if (min_pos == std::string::npos) {
            tokens.push_back(s.substr(start));
            break;
        }
        else {
            tokens.push_back(s.substr(start, min_pos - start));
            start = min_pos + delimiters[0].length();
        }
    }
    return tokens;
}
