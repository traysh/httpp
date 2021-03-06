#include <gtest/gtest.h>

#include "http/requestparser.hpp"
#include "connection/connection_mock.hpp"
#include "connection/streambuffer.hpp"

using RequestParserTest = ::testing::Test;

namespace {
TEST_F(RequestParserTest, WellFormattedRequestLine) {
    Connection::ConnectionMock connection({"GET / HTTP/1.1\r\n\r\n"});
    Connection::StreamBuffer buffer(connection);
    HTTP::RequestParser parser(buffer);

    HTTP::Request request;
    auto result = parser.Parse(request);

    EXPECT_EQ(result,  decltype(parser)::Result::Success);
    EXPECT_EQ(request.Method, HTTP::MethodType::Get);
    EXPECT_EQ(request.Path, "/");
    EXPECT_EQ(request.Protocol, Server::ProtocolType::HTTP);
    EXPECT_EQ(request.ProtocolVersion, "1.1");
}

TEST_F(RequestParserTest, SlowClientRequestLine) {
    Connection::ConnectionMock connection({"GE"});
    Connection::StreamBuffer buffer(connection);
    HTTP::RequestParser parser(buffer);

    HTTP::Request request;
    auto result = parser.Parse(request);

    EXPECT_EQ(result, decltype(parser)::Result::NoInputData);
    
    connection.PushData({"T / HTTP/1.1\r\n\r\n"});
    result = parser.Parse(request);

    EXPECT_EQ(result, decltype(parser)::Result::Success);
}

TEST_F(RequestParserTest, UnprocessableRequestLine) {
    Connection::ConnectionMock connection({"GET\n"});
    Connection::StreamBuffer buffer(connection);
    HTTP::RequestParser parser(buffer);

    HTTP::Request request;
    auto result = parser.Parse(request);

    EXPECT_EQ(result, decltype(parser)::Result::Failed);
    EXPECT_EQ(result, decltype(parser)::Result::Failed);
}

TEST_F(RequestParserTest, WellFormattedRequestHeader) {
    Connection::ConnectionMock connection({
            "GET / HTTP/1.1\r\n"
            "Host: localhost:9933\r\n"
            "User-Agent: curl/7.54.0\r\n"
            "Accept: */*\r\n\r\n"
    });
    Connection::StreamBuffer buffer(connection);
    HTTP::RequestParser parser(buffer);

    HTTP::Request request;
    auto result = parser.Parse(request);

    EXPECT_EQ(result, decltype(parser)::Result::Success);

    // Status line
    EXPECT_EQ(request.Method, HTTP::MethodType::Get);
    EXPECT_EQ(request.Path, "/");
    EXPECT_EQ(request.Protocol, Server::ProtocolType::HTTP);
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

TEST_F(RequestParserTest, SlowWellFormattedRequestHeader) {
    Connection::ConnectionMock connection({
            "GET / HTTP/1.1\r\n"
            "Host: localhost:9933\r\n"
            "User-Agent: cur",
    });
    Connection::StreamBuffer buffer(connection);
    HTTP::RequestParser parser(buffer);

    HTTP::Request request;
    auto result = parser.Parse(request);

    EXPECT_EQ(result, decltype(parser)::Result::NoInputData);
    
    connection.PushData({
            "l/7.54.0\r\n"
            "Accept: */*\r\n\r\n",
    });
    result = parser.Parse(request);

    EXPECT_EQ(result, decltype(parser)::Result::Success);

    // Status line
    EXPECT_EQ(request.Method, HTTP::MethodType::Get);
    EXPECT_EQ(request.Path, "/");
    EXPECT_EQ(request.Protocol, Server::ProtocolType::HTTP);
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
    Connection::ConnectionMock connection({
        "POST / HTTP/1.1\r\n"
        "Host: localhost:9933\r\n"
        "User-Agent: curl/7.54.0\r\n"
        "Accept: */*\r\n"
        "Content-Type: application/json\r\n"
        "Custom-Header: Type: Custom\r\n"
        "Content-Length: 13\r\n\r\n"
        "{\"foo\":\"bar\"}"
     });
    Connection::StreamBuffer buffer(connection);
    HTTP::RequestParser parser(buffer);

    HTTP::Request request;
    auto result = parser.Parse(request);

    EXPECT_EQ(result, decltype(parser)::Result::Success);

    // Status line
    EXPECT_EQ(request.Method, HTTP::MethodType::Post);
    EXPECT_EQ(request.Path, "/");
    EXPECT_EQ(request.Protocol, Server::ProtocolType::HTTP);
    EXPECT_EQ(request.ProtocolVersion, "1.1");

    // Headers
    ASSERT_FALSE(request.Header.find("HOST") == request.Header.end());
    EXPECT_EQ(request.Header.at("HOST"), "localhost:9933");
    ASSERT_FALSE(request.Header.find("USER-AGENT") == request.Header.end());
    EXPECT_EQ(request.Header.at("USER-AGENT"), "curl/7.54.0");
    ASSERT_FALSE(request.Header.find("ACCEPT") == request.Header.end());
    EXPECT_EQ(request.Header.at("ACCEPT"), "*/*");
    EXPECT_EQ(request.Header.size(), 6);

    // Body
    EXPECT_STREQ(request.Body.Buffer(), "{\"foo\":\"bar\"}");
}


TEST_F(RequestParserTest, NoCarriageReturnPost) {
    Connection::ConnectionMock connection({
        "POST / HTTP/1.1\n"
        "Host: localhost:9933\n"
        "User-Agent: curl/7.54.0\n"
        "Accept: */*\n"
        "Content-Type: application/json\n"
        "Content-Length: 13\n\n"
        "{\"foo\":\"bar\"}"
     });
    Connection::StreamBuffer buffer(connection);
    HTTP::RequestParser parser(buffer);

    HTTP::Request request;
    auto result = parser.Parse(request);

    EXPECT_EQ(result, decltype(parser)::Result::Success);

    // Status line
    EXPECT_EQ(request.Method, HTTP::MethodType::Post);
    EXPECT_EQ(request.Path, "/");
    EXPECT_EQ(request.Protocol, Server::ProtocolType::HTTP);
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
    EXPECT_STREQ(request.Body.Buffer(), "{\"foo\":\"bar\"}");
}

}
