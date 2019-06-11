#pragma once

#include <exception>

enum class HTTPErrorType {
    BadRequest = 400,
};

template <HTTPErrorType ErrorType>
class HTTPError : public std::exception {
public:
    HTTPError() : exception() {}

    virtual const char* what() const noexcept {
        return _message;
    }
    int code() { return static_cast<int>(ErrorType); }

private:
    static constexpr const char* _message = nullptr;
};

template<>
constexpr const char* HTTPError<HTTPErrorType::BadRequest>
    ::_message = "Bad Request";

