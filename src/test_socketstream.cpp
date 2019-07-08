#include <gtest/gtest.h>

#include <string>
#include <fstream>
#include <iostream>
#include <string.h>

#include "socketstreambuffer.hpp"
#include "connection.hpp"
#include "connection_mock.hpp"

constexpr size_t buffer_size = SocketStreamBuffer<Connection>::BufferSize;
using ConnectionMock = Mock::Connection<buffer_size>;
using SocketStreamTest = ::testing::Test;

TEST_F(SocketStreamTest, ReadLineReadilyAvailable) {
    const std::string requestData("GET / HTTP/1.1\r\n\r\n");

    const size_t first_newline = requestData.find('\n');
    std::string line = requestData.substr(0, first_newline);
    char readBuffer[requestData.size()];
    memset(readBuffer, 0, sizeof(readBuffer));

    ConnectionMock connection({requestData.c_str()});
    SocketStreamBuffer sbuf(connection);
    std::istream stream(&sbuf);
    stream.getline(readBuffer, sizeof(readBuffer));

    EXPECT_EQ((char*)readBuffer, line);

    stream.getline(readBuffer, sizeof(readBuffer));
    EXPECT_STREQ((char*)readBuffer, "\r");

    stream.getline(readBuffer, sizeof(readBuffer));
    EXPECT_STREQ((char*)readBuffer, "");
}

TEST_F(SocketStreamTest, ReadLineWithASmallBuffer) {
    const std::string requestData("GET / HTTP/1.1\r\n\r\n");
    const size_t buffer_size = 4;

    // readline appends '\0' to the buffer, making the string 1 char smaller
    const std::string line = requestData.substr(0, buffer_size -1);
    char readBuffer[buffer_size];
    memset(readBuffer, 0, sizeof(readBuffer));

    ConnectionMock connection({requestData.c_str()});
    SocketStreamBuffer sbuf(connection);
    std::istream stream(&sbuf);
    stream.getline(readBuffer, sizeof(readBuffer));

    EXPECT_EQ((char*)readBuffer, line);
}

TEST_F(SocketStreamTest, ReadFullLineInParts) {
    char const data[] = "GET / HTTP/1.1\r\n\r\n";
    const size_t read_buffer_size = 4;

    const std::string requestData(data);
    // readline appends '\0' to the buffer, making the string 1 char smaller
    const size_t first_newline = requestData.find('\n');
    const std::string line = requestData.substr(0, first_newline);
    char readBuffer[read_buffer_size];
    memset(readBuffer, 0, sizeof(readBuffer));

    char const (*buffer)[read_buffer_size] = reinterpret_cast<decltype(buffer)>(&data);
    Mock::Connection<read_buffer_size> connection({
            buffer[0], buffer[1], buffer[2], buffer[3], buffer[4] });
    SocketStreamBuffer sbuf(connection);
    std::istream stream(&sbuf);

    std::stringstream total;
    size_t pos = 0;
    do {
        stream.clear();
        stream.getline(readBuffer, sizeof(readBuffer));
        pos +=  stream.gcount();
        ASSERT_LT(pos, sizeof(data));
        total << readBuffer;
    } while (stream.fail());
    EXPECT_EQ(total.str(), line);
}

class SocketStreamBigLineTest : public SocketStreamTest {
public:
    static void SetUpTestCase() {
        // Read file with long line
        const char* filePath = PROJECT_ROOT "/data/biggerThatBufferLine.txt";
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        _data = new char[size + 1];
        _data[size] = 0;
        ASSERT_TRUE(file.read(_data, size));

        std::string requestData(_data);
        const size_t first_newline = requestData.find('\n');
        _line = requestData.substr(0, first_newline);

        _read_buffer_size = buffer_size * ((requestData.size() / buffer_size)
                          + 1);
        _read_buffer = new char[_read_buffer_size];
    }

    virtual void SetUp() {
        memset(_read_buffer, 0, _read_buffer_size);
    }

    static void TearDownTestCase() {
        delete[] _data;
        delete[] _read_buffer;
    }
  
protected:
    static char* _data;
    static char* _read_buffer;
    static size_t _read_buffer_size;
    static std::string _line;
};
char* SocketStreamBigLineTest::_data;
char* SocketStreamBigLineTest::_read_buffer;
size_t SocketStreamBigLineTest::_read_buffer_size;
std::string SocketStreamBigLineTest::_line;

TEST_F(SocketStreamBigLineTest, ReadLineBiggerThanBuffer) {
    char (*buffer)[buffer_size] = reinterpret_cast<decltype(buffer)>(_data);
    ConnectionMock connection({buffer[0], buffer[1]});
    SocketStreamBuffer sbuf(connection);
    std::istream stream(&sbuf);

    stream.getline(_read_buffer, _read_buffer_size);
    auto read_size = stream.gcount();
    stream.getline(&_read_buffer[read_size], _read_buffer_size - read_size); 
    ASSERT_EQ((char*)_read_buffer, _line);
}

TEST_F(SocketStreamBigLineTest, SeekAbsoluteInputPosition) {
    char (*buffer)[buffer_size] = reinterpret_cast<decltype(buffer)>(_data);
    ConnectionMock connection({buffer[0], buffer[1]});
    SocketStreamBuffer sbuf(connection);
    std::istream stream(&sbuf);

    stream.getline(_read_buffer, _read_buffer_size);
    ASSERT_EQ((char*)_read_buffer, _line);

    sbuf.pubseekpos(buffer_size - 1, std::ios_base::in);
    memset(_read_buffer, 0, _read_buffer_size);
    stream.getline(_read_buffer, _line.size() - buffer_size);
    EXPECT_EQ((char*)_read_buffer, _line.substr(buffer_size - 1,
                                               _line.size() - (buffer_size + 1))
                                              );
}

TEST_F(SocketStreamBigLineTest, SeekRelativeInputPositionFromBeggining) {
    char (*buffer)[buffer_size] = reinterpret_cast<decltype(buffer)>(_data);
    ConnectionMock connection({buffer[0], buffer[1]});
    SocketStreamBuffer sbuf(connection);
    std::istream stream(&sbuf);

    stream.getline(_read_buffer, _read_buffer_size);
    ASSERT_EQ((char*)_read_buffer, _line);

    sbuf.pubseekoff(buffer_size + 1, std::ios_base::beg, std::ios_base::in);
    memset(_read_buffer, 0, _read_buffer_size);
    stream.getline(_read_buffer, _line.size() - buffer_size);
    EXPECT_EQ((char*)_read_buffer, _line.substr(buffer_size + 1,
                                               _line.size() - (buffer_size + 1))
                                              );
}

TEST_F(SocketStreamBigLineTest, SeekRelativeInputPositionFromCurrent) {
    char (*buffer)[buffer_size] = reinterpret_cast<decltype(buffer)>(_data);
    ConnectionMock connection({buffer[0], buffer[1]});
    SocketStreamBuffer sbuf(connection);
    std::istream stream(&sbuf);

    stream.getline(_read_buffer, _read_buffer_size);
    ASSERT_EQ((char*)_read_buffer, _line);

    sbuf.pubseekoff(-static_cast<std::streampos>(buffer_size),
                    std::ios_base::cur, std::ios_base::in);
    memset(_read_buffer, 0, _read_buffer_size);
    stream.getline(_read_buffer, buffer_size);
    EXPECT_EQ((char*)_read_buffer, _line.substr(_line.size() + 1 -buffer_size,
                                                buffer_size));
}

TEST_F(SocketStreamBigLineTest, SeekRelativeInputPositionFromEnd) {
    char (*buffer)[buffer_size] = reinterpret_cast<decltype(buffer)>(_data);
    ConnectionMock connection({buffer[0], buffer[1]});
    SocketStreamBuffer sbuf(connection);
    std::istream stream(&sbuf);

    stream.getline(_read_buffer, _read_buffer_size);
    ASSERT_EQ((char*)_read_buffer, _line);

    sbuf.pubseekoff(-static_cast<std::streampos>(buffer_size),
                    std::ios_base::end, std::ios_base::in);
    memset(_read_buffer, 0, _read_buffer_size);
    stream.getline(_read_buffer, buffer_size);
    EXPECT_EQ((char*)_read_buffer, _line.substr(strlen(_data) -buffer_size, 
                                               buffer_size));
}

TEST_F(SocketStreamBigLineTest, SeekAbsoluteOutputPosition) {
    char (*buffer)[buffer_size] = reinterpret_cast<decltype(buffer)>(_data);
    ConnectionMock connection({buffer[0], buffer[1], buffer[0]});
    SocketStreamBuffer sbuf(connection);
    std::istream stream(&sbuf);

    stream.getline(_read_buffer, _read_buffer_size);
    ASSERT_EQ((char*)_read_buffer, _line);
    memset(_read_buffer, 0, _read_buffer_size);
    stream.getline(_read_buffer, _read_buffer_size);
    _read_buffer[strlen(_read_buffer)] = '\n';
    ASSERT_STREQ((char*)_read_buffer, &_data[_line.size() +1]);

    sbuf.pubseekpos(buffer_size, std::ios_base::out);
    memset(_read_buffer, 0, _read_buffer_size);
    stream.getline(_read_buffer, buffer_size);
    EXPECT_EQ((char*)_read_buffer,
              std::string(&_data[strlen(_data) - buffer_size])
                .substr(0, 2*buffer_size -strlen(_data)));
}

TEST_F(SocketStreamBigLineTest, SeekRelativeOutputPositionFromBeggining) {
    char (*buffer)[buffer_size] = reinterpret_cast<decltype(buffer)>(_data);
    ConnectionMock connection({buffer[0], buffer[1], buffer[0]});
    SocketStreamBuffer sbuf(connection);
    std::istream stream(&sbuf);

    stream.getline(_read_buffer, _read_buffer_size);
    ASSERT_EQ((char*)_read_buffer, _line);
    memset(_read_buffer, 0, _read_buffer_size);
    stream.getline(_read_buffer, _read_buffer_size);
    _read_buffer[strlen(_read_buffer)] = '\n';
    ASSERT_STREQ((char*)_read_buffer, &_data[_line.size() +1]);

    sbuf.pubseekoff(buffer_size, std::ios_base::beg, std::ios_base::out);
    memset(_read_buffer, 0, _read_buffer_size);
    stream.getline(_read_buffer, buffer_size);
    EXPECT_EQ((char*)_read_buffer,
              std::string(&_data[strlen(_data) - buffer_size])
                .substr(0, 2*buffer_size -strlen(_data)));
}

// FIXME this test is a lie. After stream.clear(), the data is copied from
// the last buffer[0] from connection, so the offset in pubseekoff makes
// no difference.
TEST_F(SocketStreamBigLineTest, SeekRelativeOutputPositionFromCurrent) {
    char (*buffer)[buffer_size] = reinterpret_cast<decltype(buffer)>(_data);
    ConnectionMock connection({buffer[0], buffer[1], buffer[0], buffer[0]});
    SocketStreamBuffer sbuf(connection);
    std::istream stream(&sbuf);

    stream.getline(_read_buffer, _read_buffer_size);
    ASSERT_EQ((char*)_read_buffer, _line);
    memset(_read_buffer, 0, _read_buffer_size);
    stream.getline(_read_buffer, _read_buffer_size);
    _read_buffer[strlen(_read_buffer)] = '\n';
    ASSERT_STREQ((char*)_read_buffer, &_data[_line.size() +1]);

    sbuf.pubseekoff(-static_cast<std::streampos>(buffer_size)/2,
                    std::ios_base::cur, std::ios_base::out);
    memset(_read_buffer, 0, _read_buffer_size);
    stream.getline(_read_buffer, buffer_size);
    EXPECT_EQ(_read_buffer[0], 0);
    EXPECT_TRUE(stream.eof());
    stream.clear();
    stream.getline(_read_buffer, buffer_size);
    std::string substr = std::string(&_data[strlen(_data) - buffer_size])
                             .substr(0, 2*buffer_size -strlen(_data));
    EXPECT_EQ(_read_buffer, substr);
}

// FIXME this test is just a lie as the test above.
TEST_F(SocketStreamBigLineTest, SeekRelativeOutputPositionFromEnd) {
    char (*buffer)[buffer_size] = reinterpret_cast<decltype(buffer)>(_data);
    ConnectionMock connection({buffer[0], buffer[1], buffer[0], buffer[0]});
    SocketStreamBuffer sbuf(connection);
    std::istream stream(&sbuf);

    stream.getline(_read_buffer, _read_buffer_size);
    ASSERT_EQ((char*)_read_buffer, _line);
    memset(_read_buffer, 0, _read_buffer_size);
    stream.getline(_read_buffer, _read_buffer_size);
    _read_buffer[strlen(_read_buffer)] = '\n';
    ASSERT_STREQ((char*)_read_buffer, &_data[_line.size() +1]);

    sbuf.pubseekoff(-1.5*static_cast<std::streampos>(buffer_size),
                    std::ios_base::end, std::ios_base::out);
    memset(_read_buffer, 0, _read_buffer_size);
    stream.getline(_read_buffer, buffer_size);
    EXPECT_EQ(_read_buffer[0], 0);
    EXPECT_TRUE(stream.eof());
    stream.clear();
    stream.getline(_read_buffer, buffer_size);
    std::string substr = std::string(&_data[strlen(_data) - buffer_size])
                             .substr(0, 2*buffer_size -strlen(_data));
    EXPECT_EQ((char*)_read_buffer, substr);
}

