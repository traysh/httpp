#pragma once

#include <sstream>
#include <istream>
#include <memory>
#include <algorithm>
#include <vector>
#include <map>

#include "connection.hpp"

// TODO rename to SocketStreamBuffer
template<class T = Connection::Ptr>
class SocketStreamBuffer : public std::streambuf {
public:
    const static size_t BufferSize = 1024; // FIXME

    SocketStreamBuffer(T& connection)
        : std::streambuf(), _connection(std::move(connection)) {}


protected:
    constexpr static std::ios_base::openmode any = std::ios_base::in
                                                 | std::ios_base::out;
    virtual inline std::streampos seekpos(
            std::streampos sp,
            std::ios_base::openmode which = any) override {

        size_t buffer_index = sp / BufferSize;
        char* buffer = _buffer[buffer_index];
        size_t buffer_pos = sp % BufferSize;

        if (which & std::ios_base::in) {
            setg(buffer, buffer + buffer_pos, inputEnd(buffer_index));
        }
        if (which & std::ios_base::out) {
            setp(buffer, buffer + BufferSize);
            pbump(buffer_pos);
        }

        return sp;
    }

    virtual inline std::streampos seekoff(
            std::streamoff off, std::ios_base::seekdir way,
            std::ios_base::openmode which = any) override {

        size_t buffer_offset = 0;
        size_t reference_offset = 0;

        if (way == std::ios_base::cur) {
            if (which & std::ios_base::in) {
                buffer_offset = _addressToBuffer.at(eback()) * BufferSize;
                reference_offset = gptr() - eback();
            }
            else {
                buffer_offset = _addressToBuffer.at(pbase()) * BufferSize;
                reference_offset = pptr() - pbase();
            }
        }
        else if (way == std::ios_base::end) {
            buffer_offset = _addressToBuffer.at(_buffer.back()) * BufferSize;

            if (which & std::ios_base::in) {
                reference_offset = _validLimit - _buffer.back();
            }
            else {
                reference_offset = epptr() - _buffer.back();
            }
        }

        size_t sp = buffer_offset + reference_offset + (int)off;
        return seekpos(sp, which);
    }

    virtual std::streamsize showmanyc() override {
        if (eback() == nullptr) {
            return 0;
        }

        size_t input_buffer_i = _addressToBuffer.at(eback());
        size_t output_buffer_i =_addressToBuffer.at(pbase());
        size_t delta = (pptr() - pbase()) - (gptr() - eback());
        delta += (output_buffer_i - input_buffer_i) * BufferSize;
        return delta; 
    }

    virtual int underflow() override {
        if (_buffer.size() == 0) {
            overflow();
            setg(_buffer[0], _buffer[0], _buffer[0]);
        }
        else if (pptr() == _buffer.back() + BufferSize) {
            overflow();
        }
        else if (pptr() == epptr()) {
            const size_t current_buffer_i = _addressToBuffer.at(pbase());
            char* buffer = _buffer[current_buffer_i + 1];
            setp(buffer, buffer + BufferSize);
        }

        if (gptr() == eback() + BufferSize) {
            const size_t current_buffer_i = _addressToBuffer.at(eback());
            char* buffer = _buffer[current_buffer_i + 1];
            setg(buffer, buffer, inputEnd(current_buffer_i + 1));
        }

        if (gptr() < egptr()) {
            return *gptr();
        }

        size_t read_size = 0;
        read_size = _connection.ReadData(pptr(), epptr() - pptr());
        if (read_size == 0) {
            return EOF;
        }

        pbump(read_size);
        if (eback() == pbase() && gptr() <= epptr()) {
            setg(eback(), gptr(), std::min(egptr() + read_size, epptr()));
        }
        else {
            return EOF;
        }

        _validLimit = std::max(pptr(), _validLimit);

        return *gptr();

    }

    virtual int overflow (int c = 0) override {
        if (pbase() == nullptr || pbase() == _buffer.back()) {
            char* newBuffer = new char[BufferSize];
            _buffer.push_back(newBuffer); // FIXME check for failure
            _addressToBuffer.emplace(newBuffer, _buffer.size() -1);
            setp(newBuffer, newBuffer + BufferSize);
            _validLimit = newBuffer;
        }
        else {
            const size_t next_output_i = _addressToBuffer[pbase()] + 1;
            char* next_buffer = _buffer[next_output_i];
            setp(next_buffer, next_buffer + BufferSize);
        }
        *pptr() = c;
        return c;
    }

    virtual std::streamsize xsgetn (char* s, std::streamsize n) override {
        size_t lineSize = 0;

        while (n > 0) {
            auto* buffer = gptr();

            using std::ios_base;
            if (showmanyc() == 0 && underflow() == EOF) {
                // TODO wait for data?
                break;
            }

            const size_t read_size = showmanyc() < n ? showmanyc() : n;
            memcpy(&s[lineSize], buffer, read_size);
            lineSize += read_size;
            n -= read_size;
        }
        return lineSize;
    }

private:
    T _connection;
    std::vector<char*> _buffer;
    std::map<char*, size_t> _addressToBuffer;
    char* _validLimit = nullptr;

    inline char* inputEnd(const size_t buffer_index) {
        if (buffer_index == _buffer.size() - 1) {
            return _validLimit;
        }

        return _buffer[buffer_index] + BufferSize;
    }
};

