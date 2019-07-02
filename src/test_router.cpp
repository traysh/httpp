#include <gtest/gtest.h>

#include <utility>

#include "connection_mock.hpp"
#include "router.hpp"

using RouterTest = ::testing::Test;
using MethodType = HTTPRequest::MethodType;
using ConnectionMock = Mock::Connection<1024>;
using RouterType = Router<ConnectionMock>;

ConnectionMock connection({});
HTTPRequest request;
HTTPResponse response(connection);

struct TestControllerException {};
auto testController = [](const auto&, auto&) {
    throw TestControllerException();
};

struct TestControllerNoRequestException {};
auto testControllerNoRequest = [](auto&) {
    throw TestControllerNoRequestException();
};

TEST_F(RouterTest, GetDefaultNotFoundRoute) {
    ConnectionMock connection({});
    HTTPResponse response(connection);
    RouterType router;

    auto callTest = router.Get({"/test", MethodType::Get});
    callTest(request, response);
    response.Flush();

    EXPECT_EQ(connection.OutputBuffer(), 
              "HTTP/1.1 404 Not Found\r\n"
              "CONTENT-LENGTH: 11\r\n\r\n"
              "Not found\r\n");
}

TEST_F(RouterTest, GetCustomNotFoundRoute) {
    ConnectionMock connection({});
    HTTPResponse response(connection);
    RouterType router;

    router.SetNotFoundHandler([](HTTPResponse<ConnectionMock>& response) {
        response.Status = HTTPResponseStatus::Type::NotFound;
        response << "Route not found!";
    });

    auto callTest = router.Get({"/test", MethodType::Get});
    callTest(request, response);
    response.Flush();

    EXPECT_EQ(connection.OutputBuffer(), 
              "HTTP/1.1 404 Not Found\r\n"
              "CONTENT-LENGTH: 16\r\n\r\n"
              "Route not found!");
}

TEST_F(RouterTest, NullNoRequestCallable) {
    RouterType router;

    Controller<ConnectionMock>::NoRequestCallableType callable = nullptr;
    router.Add("/testNoRequest", MethodType::Get, callable);
    auto callNoRequestTest = router.Get({"/testNoRequest", MethodType::Get});
    EXPECT_FALSE(callNoRequestTest); 
}

TEST_F(RouterTest, CallsAddRouteAndGet) {
    RouterType router;

    router.Add("/test", MethodType::Get, testController);
    auto callTest = router.Get({"/test", MethodType::Get});
    auto callTestAlt = router.Get("/test", MethodType::Get);
    EXPECT_THROW(callTest(request, response), TestControllerException); 
    EXPECT_THROW(callTestAlt(request, response), TestControllerException); 

    router.Add("/testNoRequest", MethodType::Get, testControllerNoRequest);
    auto callNoRequestTest = router.Get({"/testNoRequest", MethodType::Get});
    auto callNoRequestTestAlt = router.Get("/testNoRequest", MethodType::Get);
    EXPECT_THROW(callNoRequestTest(request, response),
                 TestControllerNoRequestException);
    EXPECT_THROW(callNoRequestTestAlt(request, response),
                 TestControllerNoRequestException);
}

TEST_F(RouterTest, CallsAddRouteWithTupleAndGet) {
    RouterType router;

    RouterType::RouteType route({"/test", MethodType::Get, testController});
    router.Add(route);
    auto callTest = router.Get({"/test", MethodType::Get});
    EXPECT_THROW(callTest(request, response), TestControllerException); 

    RouterType::RouteType routeNoController({"/testNoRequest", MethodType::Get,
                                            testControllerNoRequest});
    router.Add(routeNoController);
    auto callNoRequestTest = router.Get({"/testNoRequest", MethodType::Get});
    EXPECT_THROW(callNoRequestTest(request, response),
                 TestControllerNoRequestException);

}

TEST_F(RouterTest, CallsAddWithMultipleRoutesAndGet) {
    RouterType router;
    router.Add({
        {"/test", MethodType::Get, testController},
        {"/testNoRequest", MethodType::Get, testControllerNoRequest}
    });

    auto callTest = router.Get({"/test", MethodType::Get});
    EXPECT_THROW(callTest(request, response), TestControllerException); 

    auto callNoRequestTest = router.Get({"/testNoRequest", MethodType::Get});
    EXPECT_THROW(callNoRequestTest(request, response),
                 TestControllerNoRequestException);
}

