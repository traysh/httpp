#pragma once

#include <memory>
#include <streambuf>

struct ZeroCopyBuffer : public std::streambuf {
    using DataPtr = char*;

    ZeroCopyBuffer()
        : _data(nullptr), _size(0) {
        reset();
    }

    ZeroCopyBuffer(DataPtr dataPtr, size_t size)
        : _data(dataPtr), _size(size) {
        reset();
    }

    ZeroCopyBuffer& operator=(ZeroCopyBuffer&& other) {
        _data = other._data;
        _size = other._size;
        reset();
        return *this;
    }

    void SetSize(size_t size) {
        _size = size;
        if (size == 0) {
            _data = nullptr;
            reset();
        }
        return;
    }

    void Reset(DataPtr dataPtr, size_t size) {
        _data = dataPtr;
        _size = size;
        reset();
    }

    char* Data() {
        return _data;
    }

    size_t Size() {
        return _size;
    }

private:
    void reset() {
        setg(_data, _data, _data + _size);
    }

    DataPtr _data;
    size_t _size;
};

