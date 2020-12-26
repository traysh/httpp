#pragma once

#include <string>
#include <map>

#include "method_type.hpp"
#include "protocol.hpp"
#include "requestpayload.hpp"

namespace HTTP {
struct Request {
    using HeadersType = std::map<std::string, std::string>;
    using ParametersType = std::map<std::string, std::string>;

    HTTP::MethodType Method;
    std::string Path;
    ProtocolType Protocol;
    std::string ProtocolVersion;
    HeadersType Header;
    RequestPayload Body;
    ParametersType parameters;
};
}
