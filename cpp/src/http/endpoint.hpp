#ifndef HTTP_ENDPOINT_HPP
#define HTTP_ENDPOINT_HPP

#include <string>
#include "method_type.hpp"

namespace HTTP {

struct Endpoint {
    std::string Path;
    MethodType Method;
};

inline bool operator<(const Endpoint& lhs, const Endpoint& rhs) {
    const auto pathCompare = lhs.Path.compare(rhs.Path);
    return pathCompare != 0 ?  pathCompare < 0 : lhs.Method < rhs.Method;
}
}
// using KeyType = std::pair<std::string, MethodType>;
#endif // HTTP_ENDPOINT_HPP 
