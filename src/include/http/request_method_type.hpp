#ifndef HTTP_REQUEST_METHOD_TYPE_HPP
#define HTTP_REQUEST_METHOD_TYPE_HPP

namespace HTTP::Request {
enum class MethodType {
    Unknown, Get, Head, Post, Put, Delete,
    Connect, Options, Trace, Patch,
};
}; // namespace HTTP::Request
#endif // HTTP_REQUEST_METHOD_TYPE_HPP
