#pragma once

#include <string>
#include <map>

#include "http/request_method_type.hpp"
#include "requestpayload.hpp"

//TODO namespace HTTP
struct HTTPRequest {
    using HeadersType = std::map<std::string, std::string>;
    using ParametersType = std::map<std::string, std::string>;

    enum class ProtocolType {
        Unknown, HTTP,
    };

    HTTP::Request::MethodType Method;
    std::string Path;
    ProtocolType Protocol;
    std::string ProtocolVersion;
    HeadersType Header;
    RequestPayload Body;
    ParametersType parameters;
};
