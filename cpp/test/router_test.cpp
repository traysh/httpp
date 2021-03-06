#include <gtest/gtest.h>

#include <utility>

#include "connection/connection_mock.hpp"
#include "http/request.hpp"
#include "server/router.hpp"

using RouterTest = ::testing::Test;
using MethodType = HTTP::MethodType;

Connection::ConnectionMock connection({});
HTTP::Request request;
HTTP::Response response(connection);

struct TestControllerException {};
auto testController = [](const auto&, auto&) {
    throw TestControllerException();
};

struct TestControllerNoRequestException {};
auto testControllerNoRequest = [](auto&) {
    throw TestControllerNoRequestException();
};

TEST_F(RouterTest, GetDefaultNotFoundRoute) {
    Connection::ConnectionMock connection({});
    HTTP::Response response(connection);
    Server::Router router;

    auto callTest = router.Get({"/test", MethodType::Get}).controller;
    callTest(request, response);
    response.Flush();

    EXPECT_EQ(connection.OutputBuffer(), 
              "HTTP/1.1 404 Not Found\r\n"
              "CONNECTION: Close\r\n\r\n"
              "Not found\r\n");
}

TEST_F(RouterTest, GetCustomNotFoundRoute) {
    Connection::ConnectionMock connection({});
    HTTP::Response response(connection);
    Server::Router router;

    router.SetNotFoundHandler([](HTTP::Response& response) {
        response.Status = HTTP::ResponseStatus::Type::NotFound;
        response << "Route not found!";
    });

    auto callTest = router.Get({"/test", MethodType::Get}).controller;
    callTest(request, response);
    response.Flush();

    EXPECT_EQ(connection.OutputBuffer(), 
              "HTTP/1.1 404 Not Found\r\n"
              "CONTENT-LENGTH: 16\r\n\r\n"
              "Route not found!");
}

TEST_F(RouterTest, NullNoRequestCallable) {
    Server::Router router;

    Server::Controller::NoRequestCallableType callable = nullptr;
    router.Add("/testNoRequest", MethodType::Get, callable);
    auto callNoRequestTest = router.Get({"/testNoRequest", MethodType::Get}).controller;
    EXPECT_FALSE(callNoRequestTest); 
}

TEST_F(RouterTest, CallsAddRouteAndGet) {
    Server::Router router;

    router.Add("/test", MethodType::Get, testController);
    auto callTest = router.Get({"/test", MethodType::Get}).controller;
    auto callTestAlt = router.Get("/test", MethodType::Get).controller;
    EXPECT_THROW(callTest(request, response), TestControllerException); 
    EXPECT_THROW(callTestAlt(request, response), TestControllerException); 

    router.Add("/testNoRequest", MethodType::Get, testControllerNoRequest);
    auto callNoRequestTest = router.Get({"/testNoRequest", MethodType::Get}).controller;
    auto callNoRequestTestAlt = router.Get("/testNoRequest", MethodType::Get).controller;
    EXPECT_THROW(callNoRequestTest(request, response),
                 TestControllerNoRequestException);
    EXPECT_THROW(callNoRequestTestAlt(request, response),
                 TestControllerNoRequestException);
}

TEST_F(RouterTest, CallsAddRouteWithTupleAndGet) {
    Server::Router router;

    Server::Route::Route route({"/test", MethodType::Get, testController});
    router.Add(route);
    auto callTest = router.Get({"/test", MethodType::Get}).controller;
    EXPECT_THROW(callTest(request, response), TestControllerException); 

    Server::Route::Route routeNoController({"/testNoRequest", MethodType::Get, testControllerNoRequest});
    router.Add(routeNoController);
    auto callNoRequestTest = router.Get({"/testNoRequest", MethodType::Get}).controller;
    EXPECT_THROW(callNoRequestTest(request, response),
                 TestControllerNoRequestException);

}

TEST_F(RouterTest, CallsAddWithMultipleRoutesAndGet) {
    Server::Router router;
    router.Add({
        {"/test", MethodType::Get, testController},
        {"/testNoRequest", MethodType::Get, testControllerNoRequest}
    });

    auto callTest = router.Get({"/test", MethodType::Get}).controller;
    EXPECT_THROW(callTest(request, response), TestControllerException); 

    auto callNoRequestTest = router.Get({"/testNoRequest", MethodType::Get}).controller;
    EXPECT_THROW(callNoRequestTest(request, response),
                 TestControllerNoRequestException);
}

