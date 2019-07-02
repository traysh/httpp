#pragma once

#include <cstring>
#include <vector>

namespace Mock {
template <int N = 1024>
struct Connection {
    const static size_t BufferSize = N;

    Connection(const std::vector<const char*>& requests)
        : _requests(requests) {}

    template<class T,
             typename = std::enable_if_t<!std::is_array<T>::value
                                         && !std::is_pointer<T>::value>
    >
    inline Connection& operator<<(const T& data) {
        const std::string text = std::to_string(data);
        *this << text;
        return *this;
    }

    template<>
    inline Connection& operator<<(const char& c) {
        _outputBuffer << c;
        return *this;
    }

    inline auto& operator<<(const std::string& text) {
        _outputBuffer << text;
        return *this;
    }

    size_t ReadData(char* buffer, size_t size) {
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
}
