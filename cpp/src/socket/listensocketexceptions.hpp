#pragma once

#include <exception>

namespace Socket {
enum class ErrorType {
    AlreadyInitialized, ConvertAddress, BindError, ListenError,
    SelectError, SetReuseAddressError, SetNoWaitError, Unready, 
};

template <ErrorType ErrorType>
class Error : public std::exception {
public:
    Error(int error_code = 0)
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
constexpr const char* Error<ErrorType::AlreadyInitialized>
    ::_message = "Socket is already initialized";

template<>
constexpr const char* Error<ErrorType::ConvertAddress>
    ::_message = "Error converting IP address";

template<>
constexpr const char* Error<ErrorType::BindError>
    ::_message = "Bind failed";

template<>
constexpr const char* Error<ErrorType::ListenError>
    ::_message = "Listen failed";

template<>
constexpr const char* Error<ErrorType::SelectError>
    ::_message = "Socket select failed";

template<>
constexpr const char* Error<ErrorType::SetReuseAddressError>
    ::_message = "Failed setting to reuse address";

template<>
constexpr const char* Error<ErrorType::SetNoWaitError>
    ::_message = "Failed setting socket O_NOWAIT";

template<>
constexpr const char* Error<ErrorType::Unready>
    ::_message = "The socket is not ready for this operation";
}
