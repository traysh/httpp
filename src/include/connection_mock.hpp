#pragma once

#include <cstring>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include "connection.hpp"

class ConnectionMock : public Connection {
public:
    const static size_t BufferSize = 1024;

    ConnectionMock(const std::vector<const char*>& requests)
        : _requests(requests) {}

    virtual ~ConnectionMock() {}

    virtual Connection& operator<<(const std::string& text) override {
        _outputBuffer << text;
        return *this;
    }

    virtual Connection& operator<<(const char& c) override {
        _outputBuffer << c;
        return *this;
    }

    virtual size_t ReadData(char* buffer, size_t size) override {
        if (_request_count == _requests.size()) {
            // Data is no yet available and this is ok
            return 0;
        }

        const char* request = _requests[_request_count++];
        size_t read_size = std::min(strnlen(request, BufferSize), size);
        memcpy(buffer, request, read_size);
        return read_size;
    }

    void PushData(const std::vector<const char*>& data) {
        _requests.insert(std::end(_requests),
                         std::begin(data), std::end(data));
    }

    auto OutputBuffer() {
        auto data = _outputBuffer.str();
        _outputBuffer.clear();
        return data;
    }

private:
    std::vector<const char*> _requests;
    size_t _request_count = 0;
    std::stringstream _outputBuffer;
};

