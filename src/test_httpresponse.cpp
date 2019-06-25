#include <gtest/gtest.h>

#include <sstream>

#include "connection_mock.hpp"
#include "httpresponse.hpp"

using HTTPResponseTest = ::testing::Test;

using ConnectionMock = Mock::Connection<1024>;
using StatusType = HTTPResponseStatus::Type;

TEST_F(HTTPResponseTest, CheckOKResponseMessage) {
    ConnectionMock connection({""});
    HTTPResponse response(connection, StatusType::OK);

    EXPECT_STREQ(response.Status.Reason, "OK");
}

TEST_F(HTTPResponseTest, CheckNotFoundResponseMessage) {
    ConnectionMock connection({""});
    HTTPResponse response(connection, StatusType::NotFound);

    EXPECT_STREQ(response.Status.Reason, "Not Found");
}

TEST_F(HTTPResponseTest, CheckOKResponseConvertToString) {
    ConnectionMock connection({""});
    HTTPResponse response(connection, StatusType::OK);

    ASSERT_STREQ(response.Status.Reason, "OK");

    std::string raw_data = "HTTP/1.1 200 OK\r\n";
    const std::string statusStr = response.Status;
    EXPECT_EQ(raw_data, response.Status);
    EXPECT_EQ(response.Status, raw_data);
}

TEST_F(HTTPResponseTest, WriteAsyncNoFlush) {
    ConnectionMock connection({""});
    HTTPResponse response(connection, StatusType::OK);

    response << "Test data: " << 3 << '\n';

    EXPECT_EQ(connection.OutputBuffer(), "");
}

TEST_F(HTTPResponseTest, WriteAsyncFlush) {
    ConnectionMock connection({""});
    HTTPResponse response(connection, StatusType::OK);

    response << "Test data: " << 3 << '\n';
    response.Flush();

    std::stringstream expected;
    expected << "HTTP/1.1 200 OK\r\n" << "CONTENT-LENGTH: 13\r\n\r\n"
             << "Test data: " << 3 << '\n';
    EXPECT_EQ(connection.OutputBuffer(), expected.str());
}

TEST_F(HTTPResponseTest, WriteSync) {
    ConnectionMock connection({""});
    HTTPResponse response(connection, StatusType::OK);

    response.Mode = HTTPResponse<ConnectionMock>::OperationMode::Sync;
    response << "Test data: " << 3 << '\n';

    std::stringstream expected;
    expected << "HTTP/1.1 200 OK\r\n" << "Test data: " << 3 << '\n';
    EXPECT_EQ(connection.OutputBuffer(), expected.str());
}

