#pragma once

#include <cstddef>
#include <memory>

class RequestPayload {
public:
    RequestPayload() {}
    RequestPayload(char* raw_data, size_t size) : _data(raw_data), _size(size) {}
    RequestPayload(std::unique_ptr<char[]> raw_data, size_t size)
        : _ownData(std::move(raw_data)), _size(size) {}

    inline bool Empty() {
        return !_data && !_ownData;
    }

    inline char* RawData() {
        return _data == nullptr ? _ownData.get() : _data;
    }

    inline void SetSize(size_t size) {
        _size = size;
    }

    inline size_t Size() {
        return _size;
    }

private:
    char* _data = nullptr;
    std::unique_ptr<char[]> _ownData;
    size_t _size;
};
