#pragma once

#include <vector>
#include <cstring>

class BufferValuePtr {
public:
    struct Chunk {
        char* data;
        size_t size;
    };

    BufferValuePtr(char* data, const size_t size)
        : BufferValuePtr(Chunk{ data, size }) {}

    BufferValuePtr(const Chunk& chunk)  {
        _chunks.push_back(chunk);
    }

    template<class ...Args>
    BufferValuePtr(Chunk chunk,  Args... args)
        : BufferValuePtr(chunk) {
            BufferValuePtr(args...);
        }

    /*
     * Operators
     */
    inline bool operator==(const char* str) const {
        size_t pos = 0;

        for (const Chunk& chunk : _chunks) {
            if (std::strncmp(chunk.data, &str[pos], chunk.size) != 0) {
                return false;
            }
            pos += chunk.size;
        }

        return true;
    }

    inline bool operator!=(const char* str) const {
        return !(*this == str);
    }

private:
    std::vector<Chunk> _chunks;
};

inline bool operator==(const char* str, const BufferValuePtr& value) {
    return value == str;
}

inline bool operator!=(const char* str, const BufferValuePtr& value) {
    return value != str;
}

