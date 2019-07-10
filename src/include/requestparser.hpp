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
#include "connection.hpp"
#include "socketstreambuffer.hpp"
#include "streamprocessor.hpp"
#include "util_string.hpp"

template<class T = Connection>
class RequestParser {
    using _ParseStep = StreamProcessor::Result(RequestParser::*)(HTTPRequest&);
    using _Iterator = typename std::list<_ParseStep>::const_iterator;

public:
    using Result = StreamProcessor::Result;

    RequestParser(SocketStreamBuffer<T>& buffer, _Iterator currentStep = _parseSequence.begin()) 
        : _buffer(buffer), _stream(&_buffer), _currentStep(currentStep) {}

    Result Parse(HTTPRequest& request);

private:
    SocketStreamBuffer<T>& _buffer;
    std::istream _stream;
    const static std::list<_ParseStep> _parseSequence;
    _Iterator _currentStep;

    Result parseRequestLine(HTTPRequest& request);
    Result parseHeaders(HTTPRequest& request);
    Result parseBody(HTTPRequest& request);

    inline Result streamGood();

    HTTPRequest::ProtocolType mapProtocol(const std::string& str);
    HTTPRequest::MethodType mapMethod(const std::string& str);
};

template<typename T>
typename RequestParser<T>::Result RequestParser<T>::streamGood() {
    if (_stream.bad()) {
        return Result::Failed;
    }
    if (_stream.fail()) {
        _buffer.Reset();
        _stream.clear();
        return Result::IncompleteInputData;
    }

    return Result::Success;
}

template<typename T>
const std::list<typename RequestParser<T>::_ParseStep>
    RequestParser<T>::_parseSequence = {
        &RequestParser<T>::parseRequestLine,
        &RequestParser<T>::parseHeaders,
        &RequestParser<T>::parseBody
    };

template<typename T>
typename RequestParser<T>::Result RequestParser<T>::Parse(HTTPRequest& request) {
    for (; _currentStep != _parseSequence.end(); ++_currentStep) {
        auto f = *_currentStep;
        auto result = (this->*f)(request);
        if (result != Result::Success) {
            return result;
        }
    }

    return Result::Success;
}

template<typename T>
typename RequestParser<T>::Result RequestParser<T>::parseRequestLine(
        HTTPRequest& request) {
    std::string method, path, protocol, protocolVersion, rest;
    std::array<std::string*, 3> values { &method, &path, &protocol };

    auto initial_pos = _stream.tellg();

    std::istream::sentry sentry(_stream);
    (void)sentry;
    StreamProcessor processor(_stream);
    for (auto value : values) {
        auto result = processor.ExtractWord(*value);
        if (result != Result::Success) {
            if (result == Result::IncompleteInputData) {
                result = Result::NoInputData;
            }
            
            _stream.seekg(initial_pos);
            return result;
        }

        // the request line must end with a newline
        if (value != values.back()) {
            if (processor.NewLine()) {
                return Result::Failed;
            }
        }
        else {
            if (!processor.NewLine() && _buffer.in_avail() == 0) {
                return Result::NoInputData;
            }
        }
    }
    getline(_stream, rest);
    auto result = streamGood();
    if (result != Result::Success) {
        return result;
    }

    const auto protocolData = Util::String::Split(protocol, '/');
    protocol = protocolData.first;
    protocolVersion = protocolData.second;

    try {
        request.Method = mapMethod(Util::String::ToUpper(method));
        request.Path = path;
        request.Protocol = mapProtocol(Util::String::ToUpper(protocol));
        request.ProtocolVersion = protocolVersion;
    }
    catch (const std::out_of_range&) {
        return Result::Failed;
    }

    return Result::Success;
}

template<typename T>
typename RequestParser<T>::Result RequestParser<T>::parseHeaders(HTTPRequest& request) {
    std::istream::sentry sentry(_stream);
    (void)sentry;
    StreamProcessor processor(_stream);
    std::string rest;

    while (_stream.good() && !processor.NewLine()) {
        std::string key, value;

        auto initial_pos = _stream.tellg();

        auto separators = processor.DefaultSeparators({':'});
        auto result = processor.ExtractWord(key, false, true,  separators);
        if (result != Result::Success) {
            if (result == Result::IncompleteInputData) {
                result = Result::NoInputData;
            }
            
            _stream.seekg(initial_pos);
            return result;
        }

        if (_stream.get() != ':' || !_stream.good()) {
            return Result::Failed;
        }

        result = processor.ExtractWord(value);
        if (result != Result::Success) {
            if (result == Result::IncompleteInputData) {
                result = Result::NoInputData;
            }
 
            _stream.seekg(initial_pos);
            return result;
        }

        if (!processor.NewLine()) {
            return Result::Failed;
        }

        getline(_stream, rest);

        request.Header.insert_or_assign(key, value);
    }

    getline(_stream, rest);
    return Result::Success;
}

template<typename T>
typename RequestParser<T>::Result RequestParser<T>::parseBody(HTTPRequest& request) {
    // TODO handle "Transfer-Encoding" as supposed:
    // https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html
    const char content_length_key[] = "CONTENT-LENGTH";
    if (request.Header.find(content_length_key) == request.Header.end()) {
        return Result::Success;
    }

    // FIXME move this from here
    const size_t max_content_length = 1048576;
    size_t content_length = std::stoul(request.Header.at("CONTENT-LENGTH"));
    if (content_length == 0) {
        return Result::Success;
    }

    if (content_length > max_content_length) {
        return Result::Failed;
    }

    size_t available = static_cast<size_t>(_buffer.in_avail());
    if (request.Body.Empty() && _buffer.Continguous(content_length + 1)) {
        if (available != content_length) {
            return Result::NoInputData;
        }

        char* body = _buffer.GetInPtr();
        body[content_length] = 0;
        request.Body = RequestPayload(body, content_length);
        _buffer.pubseekoff(content_length + 1, std::ios_base::cur, std::ios_base::in);
    }
    else {
        if (request.Body.Empty()) {
            request.Body = RequestPayload(new char[content_length + 1], 0);
        }

        auto* data = request.Body.Buffer();
        _stream.read(&data[request.Body.Size()], available);
        auto read_size = _stream.gcount();
        if (read_size == 0) {
            return Result::NoInputData;
        }
        if (_stream.bad()) {
            return Result::Failed;
        }

        request.Body.SetSize(request.Body.Size() + static_cast<size_t>(read_size));
        data[content_length] = 0;

        if (request.Body.Size() < content_length) {
            return Result::IncompleteInputData;
        }
    }

    return Result::Success;
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

