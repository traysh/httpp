#include <gtest/gtest.h>

#include "connection_mock.hpp"
#include "requesthandler.hpp"

using RequestHandlerTest = ::testing::Test;

TEST_F(RequestHandlerTest, ParseRequest) {

    Mock::Connection connection({"GET / HTTP/1.1\r\n\r\n"});
    RequestHandler handler(connection);

    using State = RequestHandler<decltype(connection)>::State;
    auto state = handler.Process();
    EXPECT_EQ(state, State::Succeed);
}

