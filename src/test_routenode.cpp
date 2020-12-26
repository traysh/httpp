#include <gtest/gtest.h>

#include "connection_mock.hpp"
#include "http/endpoint.hpp"
#include "http/method_type.hpp"
#include "route_node.hpp"

namespace {

TEST(RouteNodeTest, InitiallyEmpty) {
    RouteNode node;
    EXPECT_TRUE(node.Empty());
}

TEST(RouteNodeTest, GetEmptyReturnsFalseForController) {
    RouteNode node;
    EXPECT_TRUE(node.Empty());

    const HTTP::Endpoint endpoint{"/", HTTP::MethodType::Get};

    const auto& result = node.Get(endpoint);
    EXPECT_FALSE(result.Controller);
    EXPECT_TRUE(result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddOneController) {
    RouteNode node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Controller::Null();
    const HTTP::Endpoint endpoint{"/", HTTP::MethodType::Get};

    node.Add(endpoint, controller);
    const auto& result = node.Get(endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_TRUE(result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddOneChildWithOneController) {
    RouteNode node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Controller::Null();
    HTTP::Endpoint endpoint{"/child", HTTP::MethodType::Get};

    node.Add(endpoint, controller);
    const auto& result = node.Get(endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_TRUE(result.RouteParameters.empty());

    endpoint = {"/", HTTP::MethodType::Get};
    const auto& other_result = node.Get(endpoint);
    EXPECT_FALSE(other_result.Controller);
    EXPECT_TRUE(other_result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddOne2ndLevelChildWithOneController) {
    RouteNode node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Controller::Null();
    HTTP::Endpoint endpoint{"/child/inner", HTTP::MethodType::Get};

    node.Add(endpoint, controller);
    const auto& result = node.Get(endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_TRUE(result.RouteParameters.empty());

    endpoint = {"/", HTTP::MethodType::Get};
    const auto& other_result = node.Get(endpoint);
    EXPECT_FALSE(other_result.Controller);
    EXPECT_TRUE(other_result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddOneChildWithTwoControllers) {
    RouteNode node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Controller::Null();
    HTTP::Endpoint endpoint{"/child", HTTP::MethodType::Get};
    HTTP::Endpoint other_endpoint{"/child", HTTP::MethodType::Post};

    node.Add(endpoint, controller);
    node.Add(other_endpoint, controller);

    const auto& result = node.Get(endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_TRUE(result.RouteParameters.empty());

    const auto& other_result = node.Get(other_endpoint);
    EXPECT_TRUE(other_result.Controller);
    EXPECT_TRUE(other_result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddTwoChildrenWithOneControllerEach) {
    RouteNode node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Controller::Null();
    HTTP::Endpoint endpoint{"/child", HTTP::MethodType::Get};
    HTTP::Endpoint other_endpoint{"/other_child", HTTP::MethodType::Post};

    node.Add(endpoint, controller);
    node.Add(other_endpoint, controller);

    const auto& result = node.Get(endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_TRUE(result.RouteParameters.empty());

    const auto& other_result = node.Get(other_endpoint);
    EXPECT_TRUE(other_result.Controller);
    EXPECT_TRUE(other_result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddOneParameterizedChildWithOneController) {
    RouteNode node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Controller::Null();
    const HTTP::Endpoint endpoint{"/:child", HTTP::MethodType::Get};
    node.Add(endpoint, controller);

    HTTP::Endpoint get_endpoint{"/best_child", HTTP::MethodType::Get};
    const auto& result = node.Get(get_endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_FALSE(result.RouteParameters.empty());
    EXPECT_EQ(result.RouteParameters.at(endpoint.Path.substr(2)), get_endpoint.Path.substr(1));

    get_endpoint = {"/", HTTP::MethodType::Get};
    const auto& other_result = node.Get(get_endpoint);
    EXPECT_FALSE(other_result.Controller);
    EXPECT_TRUE(other_result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddOneParameterizedChildWithOne2ndLevelController) {
    RouteNode node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Controller::Null();
    const HTTP::Endpoint endpoint{"/:child/action", HTTP::MethodType::Get};
    node.Add(endpoint, controller);

    HTTP::Endpoint get_endpoint{"/best_child/action", HTTP::MethodType::Get};
    const auto& result = node.Get(get_endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_FALSE(result.RouteParameters.empty());
    EXPECT_EQ(result.RouteParameters.at(endpoint.Path.substr(2, strlen("child"))),
            get_endpoint.Path.substr(1, strlen("best_child")));

    get_endpoint = {"/", HTTP::MethodType::Get};
    const auto& other_result = node.Get(get_endpoint);
    EXPECT_FALSE(other_result.Controller);
    EXPECT_TRUE(other_result.RouteParameters.empty());

    get_endpoint = {"/best_child", HTTP::MethodType::Get};
    const auto& yet_another_result = node.Get(get_endpoint);
    EXPECT_FALSE(yet_another_result.Controller);
    EXPECT_FALSE(yet_another_result.RouteParameters.empty());
}

TEST(RouteNodeTest, AddNestedParameterizedChildren) {
    RouteNode node;
    EXPECT_TRUE(node.Empty());

    const auto& controller = Controller::Null();
    const HTTP::Endpoint endpoint{"/:child/:feature/action", HTTP::MethodType::Get};
    node.Add(endpoint, controller);

    HTTP::Endpoint get_endpoint{"/best_child/paint/action", HTTP::MethodType::Get};
    const auto& result = node.Get(get_endpoint);
    EXPECT_TRUE(result.Controller);
    EXPECT_FALSE(result.RouteParameters.empty());
    EXPECT_EQ(result.RouteParameters.at(endpoint.Path.substr(2, strlen("child"))),
            get_endpoint.Path.substr(1, strlen("best_child")));
    EXPECT_EQ(result.RouteParameters.at(endpoint.Path.substr(9, strlen("feature"))),
            get_endpoint.Path.substr(12, strlen("paint")));

    get_endpoint = {"/", HTTP::MethodType::Get};
    const auto& other_result = node.Get(get_endpoint);
    EXPECT_FALSE(other_result.Controller);
    EXPECT_TRUE(other_result.RouteParameters.empty());

    get_endpoint = {"/best_child", HTTP::MethodType::Get};
    const auto& yet_another_result = node.Get(get_endpoint);
    EXPECT_FALSE(yet_another_result.Controller);
    EXPECT_FALSE(yet_another_result.RouteParameters.empty());
}

}
