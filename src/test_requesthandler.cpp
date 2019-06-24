#include <gtest/gtest.h>

#include "connection_mock.hpp"
#include "requesthandler.hpp"

using RequestHandlerTest = ::testing::Test;
using State = RequestHandler<Mock::Connection<1024>>::State;

TEST_F(RequestHandlerTest, HandleSimpleGetRequest) {
    Mock::Connection connection({"GET / HTTP/1.1\r\n\r\n"});
    RequestHandler handler(connection);

    auto state = handler.Process();
    EXPECT_EQ(state, State::Succeed);
}

TEST_F(RequestHandlerTest, HandleSimpleSlowGetRequest) {
    Mock::Connection connection({"GET / HT"});
    RequestHandler handler(connection);

    auto state = handler.Process();
    EXPECT_EQ(state, State::Processing);

    connection.PushData({"TP/1.1\r\n\r\n"});
    state = handler.Process();
    EXPECT_EQ(state, State::Succeed);
}

/*
TEST_F(RequestHandlerTest, RegisterGETRoute) {
    Mock::Connection connection({
        "GET /test HTTP/1.1\r\n\r\n"
    });
    RequestHandler handler(connection);
}
*/
