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

TEST_F(RouterTest, CallsAddRouteAndGet) {
    RouterType router;

    router.Add("/test", MethodType::Get, testController);
    auto callTest = router.Get({"/test", MethodType::Get});
    EXPECT_THROW(callTest(request, response), TestControllerException); 

    router.Add("/testNoRequest", MethodType::Get, testControllerNoRequest);
    auto callNoRequestTest = router.Get({"/testNoRequest", MethodType::Get});
    EXPECT_THROW(callNoRequestTest(request, response),
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

