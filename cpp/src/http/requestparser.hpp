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

#include "connection/streambuffer.hpp"
#include "http/request.hpp"
#include "util/streamprocessor.hpp"
#include "util/string.hpp"

namespace HTTP {
class RequestParser {
    using _ParseStep = Util::StreamProcessor::Result(RequestParser::*)(HTTP::Request&);
    using _Iterator = typename std::list<_ParseStep>::const_iterator;

public:
    using Result = Util::StreamProcessor::Result;

    RequestParser(Connection::StreamBuffer& buffer, _Iterator currentStep = _parseSequence.begin()) 
        : _buffer(buffer), _stream(&_buffer), _currentStep(currentStep) {}

    Result Parse(HTTP::Request& request);

private:
    Connection::StreamBuffer& _buffer;
    std::istream _stream;
    const static std::list<_ParseStep> _parseSequence;
    _Iterator _currentStep;

    Result parseRequestLine(HTTP::Request& request);
    Result parseHeaders(HTTP::Request& request);
    Result parseBody(HTTP::Request& request);

    inline Result streamGood();

    Server::ProtocolType mapProtocol(const std::string& str);
    HTTP::MethodType mapMethod(const std::string& str);
};
}
