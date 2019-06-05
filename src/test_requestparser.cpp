#include <gtest/gtest.h>

#include "requestparser.hpp"

struct ConnectionMock {
    using Data = std::vector<char>;

    ConnectionMock(const std::vector<const char*>& requests)
        : _requests(requests) {}

    Data ReadData() {
        const char* request = _requests[_request_count++];
        return std::vector<char>(request,
                                 request + strlen(request));
    }

private:
    const std::vector<const char*> _requests;
    size_t _request_count = 0;
};

namespace {
TEST(RequestParser, WellFormattedRequestLine) {
    ConnectionMock connection({"GET / HTTP/1.1\r\n\r\n"});
    auto request = RequestParser::parse<ConnectionMock*>(&connection);

    EXPECT_EQ(request.Method, HTTPRequest::MethodType::Get);
    EXPECT_EQ(request.Path, "/");
    EXPECT_EQ(request.Protocol, HTTPRequest::ProtocolType::HTTP);
    EXPECT_EQ(request.ProtocolVersion, "1.1");
}

TEST(RequestParser, WellFormattedRequestHeader) {
    ConnectionMock connection({
            "GET / HTTP/1.1\r\n"
            "Host: localhost:9933\r\n"
            "User-Agent: curl/7.54.0\r\n"
            "Accept: */*\r\n\r\n"
    });
    auto request = RequestParser::parse<ConnectionMock*>(&connection);

    // Status line
    EXPECT_EQ(request.Method, HTTPRequest::MethodType::Get);
    EXPECT_EQ(request.Path, "/");
    EXPECT_EQ(request.Protocol, HTTPRequest::ProtocolType::HTTP);
    EXPECT_EQ(request.ProtocolVersion, "1.1");

    // Headers
    ASSERT_NE(request.Header.find("HOST"), request.Header.end());
    EXPECT_EQ(request.Header.at("HOST"), "localhost:9933");
    ASSERT_NE(request.Header.find("USER-AGENT"), request.Header.end());
    EXPECT_EQ(request.Header.at("USER-AGENT"), "curl/7.54.0");
    ASSERT_NE(request.Header.find("ACCEPT"), request.Header.end());
    EXPECT_EQ(request.Header.at("ACCEPT"), "*/*");
    EXPECT_EQ(request.Header.size(), 3);
}

TEST(RequestParser, WellFormattedPost) {
    ConnectionMock connection({
            "POST / HTTP/1.1\r\n"
            "Host: localhost:9933\r\n"
            "User-Agent: curl/7.54.0\r\n"
            "Accept: */*\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 13\r\n\r\n"
            "{\"foo\":\"bar\"}"
    });
    auto request = RequestParser::parse<ConnectionMock*>(&connection);

    // Status line
    EXPECT_EQ(request.Method, HTTPRequest::MethodType::Post);
    EXPECT_EQ(request.Path, "/");
    EXPECT_EQ(request.Protocol, HTTPRequest::ProtocolType::HTTP);
    EXPECT_EQ(request.ProtocolVersion, "1.1");

    // Headers
    ASSERT_NE(request.Header.find("HOST"), request.Header.end());
    EXPECT_EQ(request.Header.at("HOST"), "localhost:9933");
    ASSERT_NE(request.Header.find("USER-AGENT"), request.Header.end());
    EXPECT_EQ(request.Header.at("USER-AGENT"), "curl/7.54.0");
    ASSERT_NE(request.Header.find("ACCEPT"), request.Header.end());
    EXPECT_EQ(request.Header.at("ACCEPT"), "*/*");
    EXPECT_EQ(request.Header.size(), 5);

    // Body
    ASSERT_EQ(request.Body, "{\"foo\":\"bar\"}");
}

}
