#pragma once

#include <cstring>
#include <vector>

namespace Mock {
template <int N = 1024>
struct Connection {
    const static size_t BufferSize = N;

    Connection(const std::vector<const char*> requests)
        : _requests(requests) {}

    size_t ReadData(char* buffer, size_t size) {
        if (_request_count == _requests.size()) {
            return 0; // TODO exception
        }

        const char* request = _requests[_request_count++];
        size_t read_size = std::min(strnlen(request, BufferSize), size);
        memcpy(buffer, request, read_size);
        return read_size;
    }

private:
    const std::vector<const char*> _requests;
    size_t _request_count = 0;
};
}
