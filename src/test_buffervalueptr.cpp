#include <gtest/gtest.h>
#include <numeric>
#include <vector>

#include "buffervalueptr.hpp"

using BufferValuePtrTest = ::testing::Test;

TEST_F(BufferValuePtrTest, ContinuousDataEqualsCharArray) {
    char data[] = "Continuous data";
    BufferValuePtr value(data, sizeof(data));
    EXPECT_EQ(data, value);
    EXPECT_EQ(value, data);
    EXPECT_FALSE(data != value);
    EXPECT_FALSE(value != data);
}

TEST_F(BufferValuePtrTest, ContinuousDataDiffersCharArray) {
    char data[] = "Continuous data";
    char other_data[] = "Other data";
    BufferValuePtr value(data, sizeof(data));
    EXPECT_NE(other_data, value);
    EXPECT_NE(value, other_data);
    EXPECT_FALSE(other_data == value);
    EXPECT_FALSE(value == other_data);
}

class BufferValueSparseDataTest : public ::testing::Test {
public:
    static void SetUpTestCase() {
        _sparse_data = {
            const_cast<char*>("First array of data; "),
            const_cast<char*>("Second one; "),
            const_cast<char*>("The third; "),
            const_cast<char*>("Fourth and last."),
        };
        _sparse_data_sizes = {
            strlen(_sparse_data[0]),
            strlen(_sparse_data[1]),
            strlen(_sparse_data[2]),
            strlen(_sparse_data[3]),
        };
        const size_t data_length = std::accumulate(
            _sparse_data_sizes.begin(),
            _sparse_data_sizes.end(),
            0
        );

        _continuous_data_size = data_length;
        _continuous_data = new char[_continuous_data_size + 1];
        _continuous_data[0] = 0;
        for (const char* data_chunk : _sparse_data) {
            const size_t pos = strlen(_continuous_data);
            const size_t data_chunk_size = strlen(data_chunk);
            memcpy(&_continuous_data[pos], data_chunk, data_chunk_size);;
            _continuous_data[pos + data_chunk_size] = 0;
        }
        ASSERT_EQ(_continuous_data_size, strlen(_continuous_data));
    }

    static void TearDownTestCase() {
        delete[] _continuous_data;
        _continuous_data = nullptr;
    }

protected:
    static std::vector<char*> _sparse_data;
    static std::vector<size_t> _sparse_data_sizes;
    static char* _continuous_data;
    static size_t _continuous_data_size;
};

std::vector<char*> BufferValueSparseDataTest::_sparse_data;
std::vector<size_t> BufferValueSparseDataTest::_sparse_data_sizes;
char* BufferValueSparseDataTest::_continuous_data;
size_t BufferValueSparseDataTest::_continuous_data_size;

TEST_F(BufferValueSparseDataTest, SparseDataTest) {
    BufferValuePtr value(BufferValuePtr::Chunk{ _sparse_data[0],
                                                _sparse_data_sizes[0] },
                         BufferValuePtr::Chunk{ _sparse_data[1],
                                                _sparse_data_sizes[1] },
                         BufferValuePtr::Chunk{ _sparse_data[2],
                                                _sparse_data_sizes[2] },
                         BufferValuePtr::Chunk{ _sparse_data[3],
                                                _sparse_data_sizes[3] });

    EXPECT_EQ(value, _continuous_data);
    EXPECT_EQ(_continuous_data, value);
}
