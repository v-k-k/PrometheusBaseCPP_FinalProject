#ifndef CUSTOMEX_H
#define CUSTOMEX_H

#include <exception>
#include <string>

class CustomException : public std::exception {
public:
    explicit CustomException(const std::string& message) : message_(message) {}

    const char* what() const noexcept override {
        return message_.c_str();
    }

private:
    std::string message_;

};

#endif