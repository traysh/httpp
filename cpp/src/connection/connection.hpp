#pragma once

#include <string>

namespace Connection {
class Connection {
public:
    virtual ~Connection() {};

    virtual Connection& operator<<(const std::string& text) = 0;
    virtual Connection& operator<<(const char& c) = 0;
    virtual size_t ReadData(char* buffer, size_t size) = 0;
};

template <class T, typename = std::enable_if_t<!std::is_array<T>::value &&
                                               !std::is_pointer<T>::value> >
inline Connection& operator<<(Connection& connection, const T& data) {
    std::string text = std::to_string(data);
    connection << text;
    return connection;
}
}
