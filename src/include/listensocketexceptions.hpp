#pragma once

#include <exception>

enum class SocketErrorType {
    AlreadyInitialized, ConvertAddress, BindError, ListenError,
    SelectError, SetReuseAddressError, Unready, 
};

template <SocketErrorType ErrorType>
class SocketError : public std::exception {
public:
    SocketError(int error_code = 0)
        : exception(), _error_code(error_code) {}

    virtual const char* what() const noexcept {
        return _message;
    }
    int code() { return _error_code; }

private:
    int _error_code;
    static constexpr const char* _message = nullptr;
};

template<>
constexpr const char* SocketError<SocketErrorType::AlreadyInitialized>
    ::_message = "Socket is already initialized";

template<>
constexpr const char* SocketError<SocketErrorType::ConvertAddress>
    ::_message = "Error converting IP address";

template<>
constexpr const char* SocketError<SocketErrorType::BindError>
    ::_message = "Bind failed";

template<>
constexpr const char* SocketError<SocketErrorType::ListenError>
    ::_message = "Listen failed";

template<>
constexpr const char* SocketError<SocketErrorType::SelectError>
    ::_message = "Socket select failed";

template<>
constexpr const char* SocketError<SocketErrorType::SetReuseAddressError>
    ::_message = "Failed setting to reuse address";

template<>
constexpr const char* SocketError<SocketErrorType::Unready>
    ::_message = "The socket is not ready for this operation";

