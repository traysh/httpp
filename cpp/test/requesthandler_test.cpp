#include <gtest/gtest.h>

#include "connection/connection_mock.hpp"
#include "http/requesthandler.hpp"

using RequestHandlerTest = ::testing::Test;
using State = HTTP::RequestHandler::StateType;

TEST_F(RequestHandlerTest, HandleSimpleGetRequest) {
    Connection::ConnectionMock connection({"GET / HTTP/1.1\r\n\r\n"});
    Server::Router router;
    HTTP::RequestHandler handler(connection, router);

    auto state = handler.Process();
    EXPECT_EQ(state, State::Succeed);
}

TEST_F(RequestHandlerTest, HandleSimpleGetRequestTwice) {
    Connection::ConnectionMock connection({"GET / HTTP/1.1\r\n\r\n"});
    Server::Router router;
    HTTP::RequestHandler handler(connection, router);

    auto state = handler.Process();
    EXPECT_EQ(state, State::Succeed);

    state = handler.Process();
    EXPECT_EQ(state, State::Succeed);
}

TEST_F(RequestHandlerTest, HandleNonSenseRequestFail) {
    Connection::ConnectionMock connection({"blahshe asdasd\r\n\r\n"});
    Server::Router router;
    HTTP::RequestHandler handler(connection, router);

    auto state = handler.Process();
    EXPECT_EQ(state, State::Failed);
}

TEST_F(RequestHandlerTest, HandleSimpleSlowGetRequest) {
    Connection::ConnectionMock connection({"GET / HT"});
    Server::Router router;
    HTTP::RequestHandler handler(connection, router);

    auto state = handler.Process();
    EXPECT_EQ(state, State::WaitingForData);

    connection.PushData({"TP/1.1\r\n\r\n"});
    state = handler.Process();
    EXPECT_EQ(state, State::Succeed);
}

TEST_F(RequestHandlerTest, HandleSimpleReallySlowGetRequest) {
    Connection::ConnectionMock connection({"GET / HT"});
    Server::Router router;
    HTTP::RequestHandler handler(connection, router);

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
    Connection::ConnectionMock connection(
        {"GET / HTTP/1.1\r\n"
         "Header: Tr"});
    Server::Router router;
    HTTP::RequestHandler handler(connection, router);

    auto state = handler.Process();
    EXPECT_EQ(state, State::WaitingForData);

    connection.PushData({"ue\r\n\r\n"});
    state = handler.Process();
    EXPECT_EQ(state, State::Succeed);
}

TEST_F(RequestHandlerTest, HandleGetRequestUncaughtException) {
    Connection::ConnectionMock connection({"GET / HTTP/1.1\r\n\r\n"});
    Server::Router router;
    HTTP::RequestHandler handler(connection, router);

    router.Add("/", HTTP::MethodType::Get, [](auto&) {
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
    Connection::ConnectionMock connection({"GET / HTTP/1.1\r\n\r\n"});
    Server::Router router;
    HTTP::RequestHandler handler(connection, router);

    router.SetInternalServerErrorHandler([](auto& response) {
        response.Status = 200;
        response << "We are weird and uncaught exception is ours "
                    "backend normal behavior";
    });
    router.Add("/", HTTP::MethodType::Get,
               [](auto&) { throw std::exception(); });

    auto state = handler.Process();

    EXPECT_EQ(connection.OutputBuffer(),
              "HTTP/1.1 200 OK\r\n"
              "CONTENT-LENGTH: 67\r\n\r\n"
              "We are weird and uncaught exception is ours "
              "backend normal behavior");
    EXPECT_EQ(state, State::Failed);
}

