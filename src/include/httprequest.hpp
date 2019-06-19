#pragma once

#include <map>

#include "requestpayload.hpp"

struct HTTPRequest {
    using HeadersType = std::map<std::string, std::string>;

    enum class MethodType {
        Unknown, Get, Head, Post, Put, Delete,
        Connect, Options, Trace, Patch,
    };

    enum class ProtocolType {
        Unknown, HTTP,
    };

    MethodType Method;
    std::string Path;
    ProtocolType Protocol;
    std::string ProtocolVersion;
    HeadersType Header;
    RequestPayload Body;
};
