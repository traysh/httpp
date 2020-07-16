#include <gtest/gtest.h>

#include "connection_mock.hpp"
#include "requesthandler.hpp"

using RequestHandlerTest = ::testing::Test;
using ConnectionMock = Mock::Connection<1024>;
using State = RequestHandler<ConnectionMock>::StateType;

TEST_F(RequestHandlerTest, HandleSimpleGetRequest) {
    Mock::Connection connection({"GET / HTTP/1.1\r\n\r\n"});
    Router<ConnectionMock>  router;
    RequestHandler handler(connection, router);

    auto state = handler.Process();
    EXPECT_EQ(state, State::Succeed);
}

TEST_F(RequestHandlerTest, HandleSimpleGetRequestTwice) {
    Mock::Connection connection({"GET / HTTP/1.1\r\n\r\n"});
    Router<ConnectionMock>  router;
    RequestHandler handler(connection, router);

    auto state = handler.Process();
    EXPECT_EQ(state, State::Succeed);

    state = handler.Process();
    EXPECT_EQ(state, State::Succeed);
}

TEST_F(RequestHandlerTest, HandleNonSenseRequestFail) {
    Mock::Connection connection({"blahshe asdasd\r\n\r\n"});
    Router<ConnectionMock>  router;
    RequestHandler handler(connection, router);

    auto state = handler.Process();
    EXPECT_EQ(state, State::Failed);
}

TEST_F(RequestHandlerTest, HandleSimpleSlowGetRequest) {
    Mock::Connection connection({"GET / HT"});
    Router<ConnectionMock>  router;
    RequestHandler handler(connection, router);

    auto state = handler.Process();
    EXPECT_EQ(state, State::WaitingForData);

    connection.PushData({"TP/1.1\r\n\r\n"});
    state = handler.Process();
    EXPECT_EQ(state, State::Succeed);
}

TEST_F(RequestHandlerTest, HandleSimpleReallySlowGetRequest) {
    Mock::Connection connection({"GET / HT"});
    Router<ConnectionMock>  router;
    RequestHandler handler(connection, router);

    auto state = handler.Process();
    EXPECT_EQ(state, State::WaitingForData);

    connection.PushData({""});
    state = handler.Process();
    EXPECT_EQ(state, State::WaitingForData);

    connection.PushData({"TP/1.1\r\n\r\n"});
    state = handler.Process();
    EXPECT_EQ(state, State::Succeed);
}


TEST_F(RequestHandlerTest, HandleSlowGetRequest) {
    Mock::Connection connection({"GET / HTTP/1.1\r\n"
                                 "Header: Tr"});
    Router<ConnectionMock>  router;
    RequestHandler handler(connection, router);

    auto state = handler.Process();
    EXPECT_EQ(state, State::WaitingForData);

    connection.PushData({"ue\r\n\r\n"});
    state = handler.Process();
    EXPECT_EQ(state, State::Succeed);
}

TEST_F(RequestHandlerTest, HandleGetRequestUncaughtException) {
    Mock::Connection connection({"GET / HTTP/1.1\r\n\r\n"});
    Router<ConnectionMock>  router;
    RequestHandler handler(connection, router);

    router.Add("/", HTTP::Request::MethodType::Get, [](auto&) {
        throw std::exception();
    });

    auto state = handler.Process();

    EXPECT_EQ(connection.OutputBuffer(),
              "HTTP/1.1 500 Internal Server Error\r\n"
              "CONNECTION: Close\r\n\r\n"
              "Internal server error\r\n");
    EXPECT_EQ(state, State::Failed);
}

TEST_F(RequestHandlerTest, CustomHandleGetRequestUncaughtException) {
    Mock::Connection connection({"GET / HTTP/1.1\r\n\r\n"});
    Router<ConnectionMock>  router;
    RequestHandler handler(connection, router);

    router.SetInternalServerErrorHandler([](auto& response){
        response.Status = 200;
        response << "We are weird and uncaught exception is ours "
                    "backend normal behavior";
    });
    router.Add("/", HTTP::Request::MethodType::Get, [](auto&) {
        throw std::exception();
    });

    auto state = handler.Process();

    EXPECT_EQ(connection.OutputBuffer(),
              "HTTP/1.1 200 OK\r\n"
              "CONTENT-LENGTH: 67\r\n\r\n"
              "We are weird and uncaught exception is ours "
              "backend normal behavior");
    EXPECT_EQ(state, State::Failed);
}

