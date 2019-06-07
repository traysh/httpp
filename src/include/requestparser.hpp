#pragma once

#include <algorithm>
#include <cctype>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "httprequest.hpp"
#include "connection.hpp"
#include "socketstreambuffer.hpp"
#include "util_string.hpp"
#include "util_stream.hpp"

template<class T = Connection>
class RequestParser {
public:
    RequestParser(T& connection) : _buffer(connection) {}

    HTTPRequest parse();

private:
    SocketStreamBuffer<T> _buffer;

    void parseRequestLine(std::iostream& stream, HTTPRequest& request);
    void parseHeaders(std::iostream& stream, HTTPRequest& request);
    void parseBody(std::iostream& stream, HTTPRequest& request);

    HTTPRequest::ProtocolType mapProtocol(const std::string& str);
    HTTPRequest::MethodType mapMethod(const std::string& str);
};


template<typename T>
HTTPRequest RequestParser<T>::parse() {
    std::iostream dataStream(&_buffer);

    HTTPRequest request;

    parseRequestLine(dataStream, request);
    parseHeaders(dataStream, request);
    parseBody(dataStream, request);

    return request;
}

template<typename T>
void RequestParser<T>::parseRequestLine(std::iostream& stream, HTTPRequest& request) {
    std::string method, path, protocol, protocolVersion, rest;
    stream >> method >> path >> protocol;
    getline(stream, rest);

    const auto protocolData = Util::String::Split(protocol, '/');
    protocol = protocolData.first;
    protocolVersion = protocolData.second;

    request.Method = mapMethod(Util::String::ToUpper(method));
    request.Path = path;
    request.Protocol = mapProtocol(Util::String::ToUpper(protocol));
    request.ProtocolVersion = protocolVersion;
}

template<typename T>
void RequestParser<T>::parseHeaders(std::iostream& stream, HTTPRequest& request) {
    int next_char;

    for (next_char = stream.get();
         stream.good() && !Util::Stream::NewLine(next_char, stream);
         next_char = stream.get()) {
        std::stringstream key, value;
        std::string rest;

        std::cout << "next_char: " << (char)next_char << '(' << next_char << ')' << std::endl;

        for (;
             stream.good() && next_char != '\n' && next_char != ':';
             next_char = stream.get()) {
            
            key << static_cast<char>(::toupper(next_char));
        }

        if (next_char != ':') {
            // TODO error
        }

        for (next_char = stream.get();
             std::isblank(next_char);
             next_char = stream.get()) {}

        for (;
             stream.good() && !Util::Stream::NewLine(next_char, stream);
             next_char = stream.get()) {

            value << static_cast<char>(next_char);
        }

        if (next_char != '\n') {
            getline(stream, rest);
        }

        request.Header.insert_or_assign(key.str(), value.str());
    }
}

template<typename T>
void RequestParser<T>::parseBody(std::iostream& stream,
                             HTTPRequest& request) {
    // FIXME check content-length
    stream << '\0';

    std::stringbuf rest;
    stream >> &rest;
    request.Body = rest.str();
}

template<typename T>
HTTPRequest::MethodType RequestParser<T>::mapMethod(const std::string& str) {
    const static std::map<std::string, HTTPRequest::MethodType> m {
        { "GET", HTTPRequest::MethodType::Get },
        { "HEAD", HTTPRequest::MethodType::Head },
        { "POST", HTTPRequest::MethodType::Post },
        { "PUT", HTTPRequest::MethodType::Put },
        { "DELETE", HTTPRequest::MethodType::Delete },
        { "CONNECT", HTTPRequest::MethodType::Connect },
        { "OPTIONS", HTTPRequest::MethodType::Options },
        { "TRACE", HTTPRequest::MethodType::Trace },
        { "PATCH", HTTPRequest::MethodType::Patch },
    };

    HTTPRequest::MethodType type;
    try {
        type = m.at(str);
    }
    catch (const std::out_of_range& e) {
        return HTTPRequest::MethodType::Unknown;
    }

    return type;
}

template<typename T>
HTTPRequest::ProtocolType RequestParser<T>::mapProtocol(const std::string& str) {
    const static std::map<std::string, HTTPRequest::ProtocolType> m {
        { "HTTP", HTTPRequest::ProtocolType::HTTP },
    };

    HTTPRequest::ProtocolType type;
    try {
        type = m.at(str);
    }
    catch (const std::out_of_range& e) {
        return HTTPRequest::ProtocolType::Unknown;
    }

    return type;
}

