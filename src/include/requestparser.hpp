#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <unistd.h>

#include "httpexceptions.hpp"
#include "httprequest.hpp"
#include "socketstreambuffer.hpp"
#include "streamprocessor.hpp"
#include "util_string.hpp"

class Connection;

class RequestParser {
    using _ParseStep = StreamProcessor::Result(RequestParser::*)(HTTPRequest&);
    using _Iterator = typename std::list<_ParseStep>::const_iterator;

public:
    using Result = StreamProcessor::Result;

    RequestParser(SocketStreamBuffer& buffer, _Iterator currentStep = _parseSequence.begin()) 
        : _buffer(buffer), _stream(&_buffer), _currentStep(currentStep) {}

    Result Parse(HTTPRequest& request);

private:
    SocketStreamBuffer& _buffer;
    std::istream _stream;
    const static std::list<_ParseStep> _parseSequence;
    _Iterator _currentStep;

    Result parseRequestLine(HTTPRequest& request);
    Result parseHeaders(HTTPRequest& request);
    Result parseBody(HTTPRequest& request);

    inline Result streamGood();

    HTTPRequest::ProtocolType mapProtocol(const std::string& str);
    HTTP::Request::MethodType mapMethod(const std::string& str);
};

