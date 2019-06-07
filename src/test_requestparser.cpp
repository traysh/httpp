#include <gtest/gtest.h>

#include "requestparser.hpp"
#include "connection_mock.hpp"

using RequestParserTest = ::testing::Test;

namespace {
TEST_F(RequestParserTest, WellFormattedRequestLine) {
    Mock::Connection connection({"GET / HTTP/1.1\r\n\r\n"});
    RequestParser parser(connection);
    auto request = parser.parse();

    EXPECT_EQ(request.Method, HTTPRequest::MethodType::Get);
    EXPECT_EQ(request.Path, "/");
    EXPECT_EQ(request.Protocol, HTTPRequest::ProtocolType::HTTP);
    EXPECT_EQ(request.ProtocolVersion, "1.1");
}

TEST_F(RequestParserTest, WellFormattedRequestHeader) {
    Mock::Connection connection({
            "GET / HTTP/1.1\r\n"
            "Host: localhost:9933\r\n"
            "User-Agent: curl/7.54.0\r\n"
            "Accept: */*\r\n\r\n"
    });
    RequestParser parser(connection);
    auto request = parser.parse();

    // Status line
    EXPECT_EQ(request.Method, HTTPRequest::MethodType::Get);
    EXPECT_EQ(request.Path, "/");
    EXPECT_EQ(request.Protocol, HTTPRequest::ProtocolType::HTTP);
    EXPECT_EQ(request.ProtocolVersion, "1.1");

    // Headers
    ASSERT_FALSE(request.Header.find("HOST") == request.Header.end());
    EXPECT_EQ(request.Header.at("HOST"), "localhost:9933");
    ASSERT_FALSE(request.Header.find("USER-AGENT") == request.Header.end());
    EXPECT_EQ(request.Header.at("USER-AGENT"), "curl/7.54.0");
    ASSERT_FALSE(request.Header.find("ACCEPT") == request.Header.end());
    EXPECT_EQ(request.Header.at("ACCEPT"), "*/*");
    EXPECT_EQ(request.Header.size(), 3);
}

TEST_F(RequestParserTest, WellFormattedPost) {
    Mock::Connection connection({
        "POST / HTTP/1.1\r\n"
        "Host: localhost:9933\r\n"
        "User-Agent: curl/7.54.0\r\n"
        "Accept: */*\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 13\r\n\r\n"
        "{\"foo\":\"bar\"}"
     });
    RequestParser parser(connection);
    auto request = parser.parse();
    // Status line
    EXPECT_EQ(request.Method, HTTPRequest::MethodType::Post);
    EXPECT_EQ(request.Path, "/");
    EXPECT_EQ(request.Protocol, HTTPRequest::ProtocolType::HTTP);
    EXPECT_EQ(request.ProtocolVersion, "1.1");

    // Headers
    ASSERT_FALSE(request.Header.find("HOST") == request.Header.end());
    EXPECT_EQ(request.Header.at("HOST"), "localhost:9933");
    ASSERT_FALSE(request.Header.find("USER-AGENT") == request.Header.end());
    EXPECT_EQ(request.Header.at("USER-AGENT"), "curl/7.54.0");
    ASSERT_FALSE(request.Header.find("ACCEPT") == request.Header.end());
    EXPECT_EQ(request.Header.at("ACCEPT"), "*/*");
    EXPECT_EQ(request.Header.size(), 5);

    // Body
    ASSERT_EQ(request.Body, "{\"foo\":\"bar\"}");
}


TEST_F(RequestParserTest, NoCarriageReturnPost) {
    Mock::Connection connection({
        "POST / HTTP/1.1\n"
        "Host: localhost:9933\n"
        "User-Agent: curl/7.54.0\n"
        "Accept: */*\n"
        "Content-Type: application/json\n"
        "Content-Length: 13\n\n"
        "{\"foo\":\"bar\"}"
     });
    RequestParser parser(connection);
    auto request = parser.parse();
    // Status line
    EXPECT_EQ(request.Method, HTTPRequest::MethodType::Post);
    EXPECT_EQ(request.Path, "/");
    EXPECT_EQ(request.Protocol, HTTPRequest::ProtocolType::HTTP);
    EXPECT_EQ(request.ProtocolVersion, "1.1");

    // Headers
    ASSERT_FALSE(request.Header.find("HOST") == request.Header.end());
    EXPECT_EQ(request.Header.at("HOST"), "localhost:9933");
    ASSERT_FALSE(request.Header.find("USER-AGENT") == request.Header.end());
    EXPECT_EQ(request.Header.at("USER-AGENT"), "curl/7.54.0");
    ASSERT_FALSE(request.Header.find("ACCEPT") == request.Header.end());
    EXPECT_EQ(request.Header.at("ACCEPT"), "*/*");
    EXPECT_EQ(request.Header.size(), 5);

    // Body
    ASSERT_EQ(request.Body, "{\"foo\":\"bar\"}");
}

}
